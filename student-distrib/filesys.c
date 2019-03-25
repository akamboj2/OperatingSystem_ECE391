#include "filesys.h"
#include "lib.h"

/*
 * read_dentry_by_name
 *   DESCRIPTION: find the data entry by name and return data entry as with the file name,
 *                file type, and inode number for the file,
 *   INPUTS: fname-- pointer to a string as a character(int) array
 *   OUTPUTS: dentry-- pointer to data entry
 *   RETURN VALUE: -1 on failure, 0 on success
 *   SIDE EFFECTS: dentry will be undefined (probs pointing to end of boot block) if entry not in list!
 */
int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){
    int count=0;
    int amt_dentrys=63;
    dentry = (dentry_t*) filesys_addr; //filesys_addr is global varialbe in kernel.c
    dentry++;//skip the initial boot block entry

    for (count=0; count<amt_dentrys; count++){
        //name=dentry; //this should make the frist 32 bytes after dentry equal to name
        if (!strncmp(dentry->file_n,(int8_t*)fname,32)){
            return 0;//success! dentry should alrady be pointing at correct thing
        }
    }
    return -1; //fail if it made it to the end.
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: find the data entry by index into bootblock and return data entry as with the file name,
 *                file type, and inode number for the file,
 *   INPUTS: fname-- pointer to a string as a character(int) array
 *   OUTPUTS: dentry-- pointer to data entry
 *   RETURN VALUE: -1 on failure, 0 on success
 *   SIDE EFFECTS: none
 */
int read_dentry_by_index (uint32_t index, dentry_t* dentry){
    if (index<0 || index >63){//64 data entries so if index is >63 it is out of bounds
        return -1;
    }

    dentry = (dentry_t*) filesys_addr;//cast the integer as a pointer (mem address)
    dentry+=index; //i think index is including first 64 bytes of info otherwise change this to dentry+=index+1 if not
    return 0;

}

/*
 * read_data
 *   DESCRIPTION: read length bytes of data from file at inode to buf. begin reading at offset bytes into the file
 *   INPUTS: inode--index into inodes, offset--where to start reaeding from
 *   OUTPUTS: buf--return file data in this buffer
 *   RETURN VALUE: -1 on failure, 0 on end of file, else number of bytes read and placed into buffer
 *   SIDE EFFECTS: buf may not point to where it started when returning
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    // dentry_t* dentry;
    // if(dentry=read_dentry_by_index(inode,dentry)){
    //     return -1; //fails if inode doesn't exist or read_dentry_by_index can't find it
    // }///^don't do that bc that function is index into bootblock not inode number

    int amt_inodes=63;
    int* inode_block= (int*)filesys_addr;


    inode_block+=MEM_SIZE_4kB/4; //skip boot block
    //need to divide by 4 because it's pointer arithmenic and int is 4 bytes
    inode_block+=inode*MEM_SIZE_4kB/4;
    //now inode_block should be pointing to correct inode

    int file_length=*inode_block;//the first thing in inode_block is 4B int of length of file in bytes
    if (offset>file_length){ return -1; }

    int skip_dbs=offset/MEM_SIZE_4kB; //if your offset is more than 4kb u have to know which data blcok to go to
    int offset_indb=offset % MEM_SIZE_4kB; //how far into the block to offset before beginning to read
    inode_block++; //skip length bytes in inode

    inode_block+=skip_dbs;
    //now inode_block is pointing to the correct data block #

    char* data_block= (char*) filesys_addr;
    data_block+=MEM_SIZE_4kB*(amt_inodes); //skip all 64, 4kb chunks of memory (1 bootblock + 63 inode blocks)


    int count=0; //for how many bytes are copied
    char* at_db=data_block + *inode_block*MEM_SIZE_4kB + offset_indb; //this shoud point to the start of file data (from where we want to read)
    char eof=26; //end of file character
    while(*at_db !=eof && length<=count){           //check if this is actually the end of file character
        *buf=*at_db; //do the copy
        buf++;
        at_db++;
        count++;
        if ((int)at_db % MEM_SIZE_4kB==0){//this means u are done with this data block! move to next
            inode_block++; //this should jump 4 bytes to the index of the next data block
            at_db=data_block+*inode_block*MEM_SIZE_4kB; //don't need the offset_indb bc we're reading from the begining of the next blocks
        }
    }

    return count;//this may be less then length in the case the eof was reached before length bytes were read
}



/*
 * read
 *   DESCRIPTION: reads count bytes of data from file into buf. Uses read_data.
 *                maybe uses read_dentry_by index (index is not inode number but
 *                bootblock idx).
 *                In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
                  sooner. In the case of reads to the directory, only the filename should be provided (as much as fits, or all 32 bytes), and
                  subsequent reads should read from successive directory entries until the last is reached, at which point read should
                  repeatedly return 0.
 *   INPUTS: count (bytes of data)
 *   OUTPUTS: buf(where to write data)
 *   RETURN VALUE: -1 on failure, 0 on end of file, else number of bytes read and placed into buffer
 *   SIDE EFFECTS: none
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
    //if dir_entry has not been  read before then read it by file name
    if(!dir_entry && read_dentry_by_name(FILE_NAME,dir_entry)){
        return -1; //if reading fails return fail
    }
    if ((int)dir_entry>=filesys_addr+NUM_INODES*64){
         //each directory entry is 64 bytes and there are NUM_inodes of them
         return 0; //this means it is out of the bootblock
    }

    //now read the file
    int* inode_num= (int*)dir_entry;
    inode_num+=9; //increment 36 bytes to point at inode num
    return read_data(*inode_num,0,buf, nbytes);

    dir_entry++; //move to dir entry if done (should increment by 64 bytes)
}

/*
 * write
 *   DESCRIPTION: Does nothing because this is read only system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}


/*
 * open
 *   DESCRIPTION: Initialize and temporary structures and opens a
 *                file directory (note file types). Uses read_dentry_by_name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: can only have one file open at a time
 */
int open (const uint8_t* filename){
    FILE_NAME=filename;
    dir_entry=NULL; //indicates no reads yet
    return 0;
}



/*
 * close
 *   DESCRIPTION: Undoes what you did in open or "Does nothing"--Discussion slides
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int close (int32_t fd){
    FILE_NAME=NULL;
    return 0;
}


/*
Questions:
FIRST GET IT TO COMPILE AND RUN!
1) How do we have more than one file open and reading at a time? what's the point of that?
2) for read_dentry by index. do we add by 1?
3) WHAT IS FD IN READ?????
*/



// The three routines provided by the file system module return -1 on failure, indicating a non-existent file or invalid
// index in the case of the first two calls, or an invalid inode number in the case of the last routine. Note that the directory
// entries are indexed starting with 0. Also note that the read data call can only check that the given inode is within the
// valid range. It does not check that the inode actually corresponds to a file (not all inodes are used). However, if a bad
// data block number is found within the file bounds of the given inode, the function should also return -1.

// When successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
// type, and inode number for the file, then return 0. The last routine works much like the read system call, reading up to
// length bytes starting from position offset in the file with inode number inode and returning the number of bytes
// read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached.
