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


int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry){

}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){

}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

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
int file_open(){

}


/*
 * file_close
 *   DESCRIPTION: Undoes what you did in open or "Does nothing"--Discussion slides
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 *   SIDE EFFECTS: none
 */
int file_close(){
    return 0;
}

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
int file_read(int count, char* buf);{

}

/*
 * file_write
 *   DESCRIPTION: Does nothing because this is read only system
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1
 *   SIDE EFFECTS: none
 */
int file_close(){
    return -1;
}
