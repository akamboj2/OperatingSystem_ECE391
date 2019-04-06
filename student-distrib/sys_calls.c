#include "sys_calls.h"
#include "types.h"
#include "lib.h"

/*halt
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t halt (uint8_t status){
  printf("IN HALT!\n");
  return 0;
}

/*execute
 * Description: Initialize file_array to have all flags=0 (indicate not present), and set fd=0 to std out
  and fd=1 to stdin
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t execute (const uint8_t* command){
//  printf("In execute!\n");
  return 0;
}

/*read
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
  return 0;
}

/*write
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
  return 0;
}

/*open
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns file descriptor index on succes, -1 on failure
  failure is when the file array is full, filename is garbage/not there, etc.
 * Side Effects: none
 */
int32_t open (const uint8_t* filename){
//  printf("in open\n");
  dentry_t entry;
  if(read_dentry_by_name(filename, &entry) || file_arr_size>=8){ //find dentry and return -1 if it fails
    return -1;
  }
  // printf("FOUND file: %s\n",entry.file_name);
  //now find empty entry in file_array
  int fd =0; //this is loop counter and also holds empty index in file_array
  for(fd=0;fd<file_arr_size;fd++){
    if (!(FD_FLAG_PRESENT & file_array[fd].flags)){ //if it is not present
      file_array[fd].flags=FD_FLAG_PRESENT;
      break;
    }
  }//at the end of this i should be index of empty entry

  //determine file type
  //File types are 0 for...(RTC), 1 for the directory, and 2 for a regular file --from docs
//  printf("file type: %d\n",entry.file_type);

  switch(entry.file_type){
    case 0:
  //    file_array[fd].fops_table= &rtc_table;
      file_array[fd].inode=0;
      file_array[fd].flags+=FD_FLAG_RTC;
      break;
    case 1:
      //file_array[fd].fops_table= &dir_table;
      file_array[fd].inode=0;
      file_array[fd].flags+=FD_FLAG_DIRECTORY;
      break;
    case 2:
      file_array[fd].fops_table= &file_table;//&(*file_table[4]);
      file_array[fd].inode=entry.inode_num;
      file_array[fd].flags+=FD_FLAG_FILE;
      break;
  }
  file_array[fd].f_pos=0; //file position should start at beginning for all file types
//  printf("now file_array[%d] has flags %d\n",fd,file_array[fd].flags);
  file_arr_size+=1;
  //finally return fd
  return 0;
}

/*close
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t close (int32_t fd){
  return 0;
}

/*getargs
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
  return 0;
}

/*vidmap
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t vidmap (uint8_t** screen_start){
  return 0;
}

/*set_handler
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t set_handler (int32_t signum, void* handler_address){
  return 0;
}

/*sigreturn
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t sigreturn (void){
  return 0;
}
