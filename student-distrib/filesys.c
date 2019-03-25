#include "filesys.h"

/*
 * read data 
 *   DESCRIPTION: Given the (x,y) map pixel coordinate of the leftmost
 *                pixel of a line to be drawn on the screen, this routine
 *                produces an image of the line.  Each pixel on the line
 *                is represented as a single byte in the image.
 *   INPUTS: (x,y) -- leftmost pixel of line to be drawn
 *   OUTPUTS: buf -- buffer holding image data for the line
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

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
    uint8_t name[32];//name data field is 32B in dir. entry
    int count=0;
    int amt_dentrys=63;
    dentry = filesys_addr; //filesys_addr is global varialbe in kernel.c
    dentry++;//skip the initial boot block entry

    for (count=0;count<dentry;count++){
        name=dentry; //this should make the frist 32 bytes after dentry equal to name
        if (fname==name){
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
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
    if (index<0 || index >63){//64 data entries so if index is >63 it is out of bounds
        return -1;
    }

    dentry = filesys_addr;
    dentry+=index;
    return 0;

}

/*
 * read_dentry_by_name
 *   DESCRIPTION: find the data entry by inode index and return data entry as with the file name, 
 *                file type, and inode number for the file, 
 *   INPUTS: fname-- pointer to a string as a character(int) array
 *   OUTPUTS: dentry-- pointer to data entry
 *   RETURN VALUE: -1 on failure, 0 on success
 *   SIDE EFFECTS: none
 */
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

}



The three routines provided by the file system module return -1 on failure, indicating a non-existent file or invalid
index in the case of the first two calls, or an invalid inode number in the case of the last routine. Note that the directory
entries are indexed starting with 0. Also note that the read data call can only check that the given inode is within the
valid range. It does not check that the inode actually corresponds to a file (not all inodes are used). However, if a bad
data block number is found within the file bounds of the given inode, the function should also return -1.
When successful, the first two calls fill in the dentry t block passed as their second argument with the file name, file
type, and inode number for the file, then return 0. The last routine works much like the read system call, reading up to
length bytes starting from position offset in the file with inode number inode and returning the number of bytes
read and placed in the buffer. A return value of 0 thus indicates that the end of the file has been reached.

/*
 * file_read
 *   DESCRIPTION: reads count bytes of data from file into buf. Uses read_data.
 *                maybe uses read_dentry_by index (index is not inode number but 
 *                bootblock idx) 
 *   INPUTS: count (bytes of data)
 *   OUTPUTS: buf(where to write data)
 *   RETURN VALUE: 0 on success, -1 on failure
 *   SIDE EFFECTS: none
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){

}

/*
 * file_write
 *   DESCRIPTION: Does nothing because this is read only system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
    return -1;
}


/*
 * file_open
 *   DESCRIPTION: Initialize and temporary structures and opens a 
 *                file directory (note file types). Uses read_dentry_by_name
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t open (const uint8_t* filename){

}



/*
 * file_close
 *   DESCRIPTION: Undoes what you did in open or "Does nothing"--Discussion slides
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int32_t close (int32_t fd){
    return 0;
}