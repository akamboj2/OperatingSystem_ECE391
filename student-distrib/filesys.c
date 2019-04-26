#include "filesys.h"
#include "lib.h"
#include "types.h"

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
    if (fname==NULL || dentry==NULL) return -1; //sanity checks
    int count=0;
    int* num_entries=(int*)filesys_addr;
    int amt_dentrys=*num_entries;
    int file_name_size=32;
    dentry_t* dentry_loop = (dentry_t*) filesys_addr; //filesys_addr is global varialbe in kernel.c
    dentry_loop++;//skip the initial boot block entry

    for (count=0; count<amt_dentrys; count++){
        //name=dentry; //this should make the frist 32 bytes after dentry equal to name
        //printf("Now at file %s\n",dentry_loop->file_name);
        if (!strncmp(dentry_loop->file_name,(int8_t*)fname, file_name_size)){//
            //*dentry=*dentry_loop;
            memcpy(dentry,dentry_loop,64);//only copy 32 bytes for 32 char file name
            return 0;//success! dentry should alrady be pointing at correct thing
        }
        dentry_loop++;
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
    int* num_entries=(int*)filesys_addr;
    int amt_dentrys=*num_entries;
  //  printf("in readbyind, index:%d, amt_dentrys:%d",index,amt_dentrys);
    if (index<0 || index >amt_dentrys){//64 data entries so if index is >63 it is out of bounds
        return -1;
    }

    dentry_t* dentry_loop = (dentry_t*) filesys_addr;//cast the integer as a pointer (mem address)
    dentry_loop++;
    dentry_loop+=index; //i think index is including first 64 bytes of info otherwise change this to dentry+=index+1 if not
    //*dentry=*dentry_loop;
    memcpy(dentry,dentry_loop,64);//only copy 31 bytes for 32 char file name
    return 0;

}

/*
 * read_data
 *   DESCRIPTION: read length bytes of data from file at inode to buf. begin reading at offset bytes into the file
 *   INPUTS: inode--index into inodes, offset--where to start reaeding from, length number of bytes to copy
 *   OUTPUTS: buf--return file data in this buffer
 *   RETURN VALUE: -1 on failure, 0 on end of file, else number of bytes read and placed into buffer
 *   SIDE EFFECTS: buf may not point to where it started when returning
 */
 int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
     // dentry_t* dentry;
     // if(dentry=read_dentry_by_index(inode,dentry)){
     //     return -1; //fails if inode doesn't exist or read_dentry_by_index can't find it
     // }///^don't do that bc that function is index into bootblock not inode number

     int* inode_block= (int*)filesys_addr;
     int amt_inodes=*(inode_block+1);//63;
     //printf("N inodes # %d\n",amt_inodes);

     inode_block+=MEM_SIZE_4kB/4; //skip boot block
     //need to divide by 4 because it's pointer arithmenic and int is 4 bytes
     inode_block+=inode*MEM_SIZE_4kB/4;
     //now inode_block should be pointing to correct inode

     //printf("first data block: length:%d num: %d\n",*inode_block,*(inode_block+1));


     int file_length=*inode_block;//the first thing in inode_block is 4B int of length of file in bytes

     if (offset>file_length){
       return 0;
     }

     int skip_dbs=offset/MEM_SIZE_4kB; //if your offset is more than 4kb u have to know which data blcok to go to
     int offset_indb=offset % MEM_SIZE_4kB; //how far into the block to offset before beginning to read
     inode_block++; //skip length bytes in inode

     inode_block+=skip_dbs;
     //now inode_block is pointing to the correct data block #

     char* data_block= (char*) filesys_addr;
     data_block+=MEM_SIZE_4kB*(amt_inodes+1); //skip all 64, 4kb chunks of memory (1 bootblock + 63 inode blocks)

     /*char testbuff[100];
     memcpy(testbuff,data_block + 6*MEM_SIZE_4kB,100);
     printf("data block 6: %s",testbuff);*/
     //printf("data block content: %s",data_block + *inode_block*MEM_SIZE_4kB);

     //printf("print by char:\n");
     uint32_t count=0; //for how many bytes are copied
     char* at_db=data_block + (*inode_block)*MEM_SIZE_4kB + offset_indb; //this shoud point to the start of file data (from where we want to read)
     //char eof=26; //end of file character
     //FIX END OF FILE
     while(count < file_length && count<length){           //check if this is actually the end of file character
         //printf("%c",*at_db);
         *buf=*at_db; //do the copy
         buf++;
         at_db++;
         count++;
         if ((int)(at_db) % MEM_SIZE_4kB==0){//thizs means u are done with this data block! move to next
             inode_block++; //this should jump 4 bytes to the index of the next data block
             at_db=data_block+(*inode_block)*MEM_SIZE_4kB; //don't need the offset_indb bc we're reading from the begining of the next blocks
         }

     }
     return count;//this may be less then length in the case the eof was reached before length bytes were read
 }



/*
 * file_read
 *   DESCRIPTION: reads count bytes of data from file into buf. Uses read_data.
 *                maybe uses read_dentry_by index (index is not inode number but
 *                bootblock idx).
 *                In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
                  sooner.
 *   INPUTS: count -- bytes of data to read
 *   OUTPUTS: buf -- where to write data
 *   RETURN VALUE: -1 on failure, 0 on end of file, else number of bytes read and placed into buffer
 *   SIDE EFFECTS: none
 */
int32_t file_read (int32_t fd, void* buf, int32_t nbytes){
  pcb_t* cur_pcb=getPCB(highest_terminal_processes[curr_terminal-1]);
  //firs some sanity checks
  //printf("in file_read: fd:%d,buf:%s,nbytes:%d",fd,buf,nbytes);
  if (nbytes<0){
    return -1;
  }
  if (nbytes==0){
    return 0;
  }

  uint32_t n = read_data(cur_pcb->file_array[fd].inode, cur_pcb->file_array[fd].f_pos, buf, nbytes);
  cur_pcb->file_array[fd].f_pos += n;
  //printf("reading file\n");
  //now read the file
  return n;
}

/*
 * file_write
 *   DESCRIPTION: Does nothing because this is read only system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int file_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}


/*
 * file_open
 *   DESCRIPTION: Opens a file by adding it to file_array (just calls open)
            DOES NOTHING! User must call open() anyways
 *   INPUTS: filename - name of file to open
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: calls open system calls in sys_calls.c
 */
int file_open (const uint8_t* filename){
  /*this looks janky but if you want the open system to call this instead
  you will have to take some code from there and put it in rtc,dir open too
  so there is no point in repeating same code and I think it's easire this way
  */
  //open(filename);
  return 0;
}



/*
 * file_close
 *   DESCRIPTION: NEVER CALL THIS! CALL SYSTEM CALL CLOSE TO ACTUALLY CLOSE A FILE!
 *          Undoes what you did in open or "Does nothing"--Discussion slides
 *   INPUTS: file_descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS:
 */
int file_close (int32_t fd){
  close(fd);
  return 0;
}


/*
 * dir_read
 *   DESCRIPTION: reads count bytes of data from file into buf. Uses
                  read_dentry_by index (index is not inode number but bootblock idx).
                  In the case of reads to the directory, only the filename should be provided (as much as fits, or all 32 bytes), and
                  subsequent reads should read from successive directory entries until the last is reached, at which point read should
                  repeatedly return 0.
 *   INPUTS: count -- bytes of data to read
 *   OUTPUTS: buf -- where to write data
 *   RETURN VALUE: -1 on failure, else number of bytes read and placed into buffer
 *   SIDE EFFECTS: none
 */
int32_t dir_read (int32_t fd, void* buf, int32_t nbytes){
  dentry_t dentry_test;
  int i = 0;
  int* num_entries=(int*)filesys_addr;
  int amt_dentrys=*num_entries;

  // printf("AMOUNT DIR ENTRIES: %d\n",amt_dentrys);
  // printf("On dir_read call:%d\n",dir_index);
  //already done reading directories then keep returning 0
  if (dir_index>=amt_dentrys)
    return 0;

  if(read_dentry_by_index(dir_index,&dentry_test)){
      return -1; //if reading fails return fail
  }
//  printf("at file: %s\n",dentry_test.file_name);
  //now read the file name into buf
  uint8_t bytes_cpy=(nbytes>FILE_NAME_SIZE ? FILE_NAME_SIZE : nbytes);
  /*uint8_t temp[33] = {'\0'};
  while(dentry_test.file_name[i] != '\0' && i < 32){
    temp[i] = dentry_test.file_name[i];
    i++;
  }temp[i] = '\n';*/
  for(i = 0; i < bytes_cpy; i++){
    if(dentry_test.file_name[i] == '\0')
      break;
  }
  memcpy(buf,dentry_test.file_name/*temp*/,i+1);
  //while(((uint8_t*)(buf))[i] != '\0'){
    //i++;
  //}((uint8_t*)(buf))[i] != '\n';

  //increment dir_index
  dir_index++;
  //printf("  Here now dir_indx is %d\n",dir_index);
  return i;
}

/*
 * dir_write
 *   DESCRIPTION: Does nothing because this is read only system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int dir_write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}


/*
 * dir_open
 *   DESCRIPTION: Initialize and temporary structures and opens a
 *                file directory (note file types). Uses read_dentry_by_name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: can only have one file open at a time
 */
int dir_open (const uint8_t* filename){
    //return open(filename);
    //uncomment this if you want to test in kernel space
    dir_index=0;
    return 0;
}



/*
 * dir_close
 *   DESCRIPTION: Undoes what you did in open or "Does nothing"--Discussion slides
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int dir_close (int32_t fd){
  //printf("Enter dir_close!\n");
    dir_index=0;
    return 0;
}

int32_t file_size(int32_t inode_num){
  int* inode_block= (int*)filesys_addr;

  inode_block+=MEM_SIZE_4kB/4; //skip boot block
  //need to divide by 4 because it's pointer arithmenic and int is 4 bytes
  inode_block+=inode_num*MEM_SIZE_4kB/4;
  return (int32_t)*inode_block;
}




//below is referring to first 3 functions
// The three routines provided by the file system module return -1 on failure, indicating a non-existent file or invalid
// index in the case of the first two calls, or an invalid inode number in the case of the last routine. Note that the directory
// entries are indexed starting with 0. Also note that the read data call can only check that the given inode is within the
// valid range. It does not check that the inode actually corresponds to a file (not all inodes are used). However, if a bad
// data block number is found within the file bounds of the given inode, the function should also return -1.

// When successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
// type, and inode number for the file, then return 0. The last routine works much like the read system call, reading up to
// length bytes starting from position offset in the file with inode number inode and returning the number of bytes
// read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached.
