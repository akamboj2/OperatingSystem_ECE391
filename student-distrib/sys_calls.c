#include "sys_calls.h"
#include "types.h"
#include "lib.h"

static ftable file_table = {&file_read, &file_write, &file_open, &file_close};
static ftable dir_table = {&dir_read, &dir_write, &dir_open, &dir_close};
static ftable rtc_table = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};

/*halt
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t halt (uint8_t status){
  printf("IN HALT!\n");
  asm volatile("" //need double %% for registers single % to specify input/output from C
                : //no outputs
                : "%ebl"() //inputs
                : ""); //clobbered registers
  return 0;
}

/*execute
 * Description: Initialize file_array to have all flags=0 (indicate not present), and set fd=0 to std out
  and fd=1 to stdin, ---AND INITIALIZEW FILE_ARR_SIZE TO 2!
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
int32_t read(int32_t fd, void* buf, int32_t nbytes){
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0)
    return -1;

  int32_t position = file_array[fd].fops_table->read(fd, buf, nbytes);
  if(position == 0 || position == -1)
    return 0;
  file_array[fd].f_pos += position;

  return position;
}

/*write
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0)
    return -1;

  if(file_array[fd].flags & FD_FLAG_FILE)
    return -1;

  int32_t position = file_array[fd].fops_table->write(fd, buf, nbytes);

  return position;
}

/*open
 * Description: finds filename in directory entries, finds empty file_array spot and
                and fills in the correct fd_struct fields in that file array, returning
                index to that array
                MAKE SURE THIS DOESN"T CALL THE FILE_OPEN FUNCTION, bc file_open calls this lol ---same for dir_open
                NOTE: IF you want it to call an open function for a specific file type
                do that in the switch statement--NOT the jump table aka don't do file_array[fd]->open()
 * Inputs: filename - file name as uint8 array of characters
 * Outputs/Return Values: Returns file descriptor index on succes, -1 on failure
  failure is when the file array is full, filename is garbage/not there, etc.
 * Side Effects: calls read_dentry_by_name
 */
int32_t open (const uint8_t* filename){
  dentry_t entry;
  //printf("file_arr_size: %d\n",file_arr_size);
  if(read_dentry_by_name(filename, &entry) || file_arr_size>=MAX_OPEN_FILES){ //find dentry and return -1 if it fails
    return -1;
  }
  //printf("in open\n");
  //now find empty entry in file_array
  int fd =0; //this is loop counter and also holds empty index in file_array
  for(fd=0;fd<MAX_OPEN_FILES;fd++){
    if (!(FD_FLAG_PRESENT & file_array[fd].flags)){ //if it is not present
      file_array[fd].flags=FD_FLAG_PRESENT;
      break;
    }
  }//at the end of this i should be index of empty entry
//  printf("Found empty file of type:%d\n",entry.file_type);
  //determine file type
  //File types are 0 for...(RTC), 1 for the directory, and 2 for a regular file --from docs
  switch(entry.file_type){
    case 0:
      rtc_open();
      file_array[fd].fops_table= &rtc_table;
      file_array[fd].inode=0;
      file_array[fd].flags+=FD_FLAG_RTC;
      break;
    case 1:
      file_array[fd].fops_table= &dir_table;
      file_array[fd].inode=0;
      file_array[fd].flags+=FD_FLAG_DIRECTORY;
      break;
    case 2:
      file_array[fd].fops_table= &file_table;//&(*file_table[4]);//&(*file_table[4]);
      file_array[fd].inode=entry.inode_num;
      file_array[fd].flags+=FD_FLAG_FILE;
      break;
  }
  file_array[fd].f_pos=0; //file position should start at beginning for all file types
  file_arr_size+=1;
  printf("Now set fd: %d to flags: %d\n",fd,file_array[fd].flags);

  //finally return fd
  return fd;
}

/*close
 * Description: close a file by setting it's flags to zero--means not not present
              then open should overwrite the rest of the stuff in the fd_struct when Opening
              next file
 * Inputs: fd- file descriptor of file to close
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
              failure: trying to open fd=0,1 (which is stdin/stdout)
 * Side Effects: uses printf for error message if invalid fd
 */
int32_t close (int32_t fd){
//  printf("Call close with fd:%d\n",fd);
  int invalid_fd_ind=2; //less than this is an invalid descriptor
  if (fd<invalid_fd_ind || fd>=MAX_OPEN_FILES){
    printf("fd=%d is invalid file_array index to open\n",fd);
    return -1;
  }
  file_array[fd].flags=0;
//  printf("    fd:%d,flags now:%d\n",fd,file_array[fd].flags);

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
