#include "sys_calls.h"
#include "types.h"
#include "lib.h"
#include "filesys.h"
#include "assembly_linkage.h"
#include "constants.h"
#include "paging.h"
#include "x86_desc.h"

static ftable file_table = {&file_read, &file_write, &file_open, &file_close};
static ftable dir_table = {&dir_read, &dir_write, &dir_open, &dir_close};
static ftable rtc_table = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};

int32_t stdin_write(int32_t a, const void* b, int32_t c){return 0;};
int32_t stdout_read(int32_t a,void* b,int32_t c){return 0;};
static ftable stdin_table = {&terminal_read,&stdin_write,&terminal_open,&terminal_close};
static ftable stdout_table = {&stdout_read,&terminal_write,&terminal_open,&terminal_close};

int32_t curr_process = 0;

pcb_t * getPCB(int32_t curr){
    return (pcb_t*)(_8MB - (curr_process)*_8KB);
}


/*halt
 * Description: jump to execute's return for the parent function
 * Inputs: return value to be transmitted to execute via eax register
 * Outputs/Return Values: Returns 0 (never actually returns! jumps to exectute's return)
 * Side Effects: writes to eax, esp, and ebp
 */
int32_t halt (uint8_t status){
  //close files in fd
  int i;
  cli();
  pcb_t* pcb_to_be_halted = getPCB(curr_process);
  pcb_t* pcb_parent = getPCB(curr_process-1);
  //correct file array. this will close any files opened by the process
  for(i=0; i<8; i++){
      if(pcb_to_be_halted->file_array[i].flags != 0)
          pcb_to_be_halted->file_array[i].fops_table->close(i);
      pcb_to_be_halted->file_array[i].flags = 0;
  }

  //restore parent paging
  curr_process--;
  pageDirectory[_4B] = ((2*_4MB) + (curr_process*_4MB)) | MAP_MASK;

  tss.esp0 = pcb_to_be_halted->esp0;
  printf("\n%x\n", tss.esp0);

  sti();

//prepare for jump to execute by finding the correct esp value in parent pcb
  asm volatile(
    ""
    "MOVL %0, %%eax;"
    "MOVL %1, %%esp;"
    "MOVL %2, %%ebp;"
    "JMP reverse_system_call;"
    : : "r" ((uint32_t)status), "r" (pcb_to_be_halted->esp), "r" (pcb_to_be_halted->ebp) : "eax"
  );
  //paste this back into eax: (uint32_t)status)
  return 0;
}

/*execute
 * Description: Initialize file_array to have all flags=0 (indicate not present), and set fd=0 to std out
  and fd=1 to stdin, ---AND INITIALIZE FILE_ARR_SIZE TO 2!
 * Inputs: buffer which represents the command the user would like to execute
 * Outputs/Return Values: Returns 0 on succes, -1 on failure, 256 if process creates a fault
 * Side Effects: writes to paging, kerel stack, and jumps to a place in memory based on entry
 * point in file about to be executed.
 */
int32_t execute (const uint8_t* command){
//  printf("In execute!\n");
  int i = 0;
  int args_flag = 0;
  //command = "fish";
  //printf("%c", *command);

  if(!command){
    return -1;
  }

  if(curr_process == 6){
    return -1;
  }
  uint8_t filename[_4B] = {'\0'};
  uint8_t data[_4B] = {'\0'};

  //parse first argument

  while(command[i] != '\0' && command[i] != ' ' && i<_4B){
    //printf(".%c.", command[i]);
    filename[i] = command[i];
    i++;
  }

  //ensure that the file actually exists

  dentry_t temp;
  if(read_dentry_by_name(filename, &temp) == -1)
    return -1;

  if(command[i] != ' ')
    args_flag = 1;

  //read data to check if executable and to find point to start executing at

  read_data(temp.inode_num, 0, data, _4B);

  //check if file is a executable
  if(data[0] != DEL || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
    return -1;

  //Paging
  pageDirectory[_4B] = (_8MB + (curr_process*_4MB)) | MAP_MASK;

  //flush tlb
  cli();
  asm volatile ("MOVL %CR3, %eax;");
  asm volatile ("MOVL %eax, %CR3;");
  sti();

  uint32_t * eip = (uint32_t*)&(data[24]); //eip holds ptr to 128MB

  //load file into program page
  int32_t nbytes = file_size(temp.inode_num);


  if(read_data(temp.inode_num, 0, (uint8_t *)PROG_LOAD_ADDR, nbytes) == -1)
    return -1;



  //PCB initialization
  pcb_t * pcb = (pcb_t *)(_8MB - (curr_process+1)*_8KB);
  if(curr_process != 0){
    pcb->parent_task =  (pcb_t *)(_8MB - (curr_process)*_8KB);
  }
  else pcb->parent_task = NULL;
  curr_process++;

  pcb->process_num = curr_process;
  pcb->eip = *eip;
  asm volatile("MOVL %%ebp, %%eax;" : "=a" (pcb->ebp));
  asm volatile("MOVL %%esp, %%eax;" : "=a" (pcb->esp));
  pcb->file_array[0].fops_table = &stdin_table;
  pcb->file_array[1].fops_table = &stdout_table;
  pcb->file_array[0].inode = -1;
  pcb->file_array[1].inode = -1;
  pcb->file_array[0].f_pos = 0;
  pcb->file_array[1].f_pos = 0;
  pcb->file_array[0].flags = FD_FLAG_PRESENT;
  pcb->file_array[1].flags = FD_FLAG_PRESENT;
  pcb->file_arr_size = 2;

  //Context Switch
  tss.ss0 = KERNEL_DS;
  pcb->esp0 = tss.esp0;
  tss.esp0 = _8MB - (curr_process-1)*_8KB - _ONE_STACK_ENTRY; //_ONE_STACK_ENTRY used to go to bottom of kernel stack for curr process

  //for eip argument, push eip found above

  context_switch((uint32_t*)*eip);

  asm volatile ("iret;");

  return 0;
}


/*read
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
  pcb_t* cur_pcb=getPCB(curr_process);
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0)
    return -1;
  //printf("IN READ YAY!\n");
  // int i =0;
  // for(i=0;i<8;i++){//this just prints everything in file array
  //   printf("at fd:%d flags=%d\n",i,cur_pcb->file_array[i].flags);
  //   if (cur_pcb->file_array[i].flags){
  //     printf(" File type: %d\n",cur_pcb->file_array[i].flags & -2);
  //     cur_pcb->file_array[i].fops_table->read(0,NULL,0);//close(i);
  //   }
  // }
  int32_t position = cur_pcb->file_array[fd].fops_table->read(fd, buf, nbytes);
  // if(position == 0 || position == -1)
  //   return 0;
  // cur_pcb->file_array[fd].f_pos += position;

  return position;
}

/*write
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
  pcb_t* cur_pcb=getPCB(curr_process);
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0)
    return -1;

  if(cur_pcb->file_array[fd].flags & FD_FLAG_FILE)
    return -1;

  int32_t position = cur_pcb->file_array[fd].fops_table->write(fd, buf, nbytes);

  return position;
}

/*open
 * Description: finds filename in directory entries, finds empty file_array spot and
                and fills in the correct fd_struct fields in that file array, returning
                index to that array
                User must use this to open stuff, not individual kernel space functions
 * Inputs: filename - file name as uint8 array of characters
 * Outputs/Return Values: Returns file descriptor index on succes, -1 on failure
  failure is when the file array is full, filename is garbage/not there, etc.
 * Side Effects: calls read_dentry_by_name
 */
int32_t open (const uint8_t* filename){
  dentry_t entry;
  pcb_t* cur_pcb=getPCB(curr_process);
  //printf("cur_pcb->file_arr_size: %d\n",cur_pcb->file_arr_size);
  if(read_dentry_by_name(filename, &entry) || cur_pcb->file_arr_size>=MAX_OPEN_FILES){ //find dentry and return -1 if it fails
    return -1;
  }
  //printf("in open\n");
  //now find empty entry in file_array
  int fd = 2; //this is loop counter and also holds empty index in file_array
  for(fd = 2;fd < MAX_OPEN_FILES;fd++){
    if (!(FD_FLAG_PRESENT & cur_pcb->file_array[fd].flags)){ //if it is not present
      cur_pcb->file_array[fd].flags=FD_FLAG_PRESENT;
      break;
    }
  }//at the end of this i should be index of empty entry
//  printf("Found empty file of type:%d\n",entry.file_type);
  //determine file type
  //File types are 0 for...(RTC), 1 for the directory, and 2 for a regular file --from docs
  switch(entry.file_type){
    case 0:
      cur_pcb->file_array[fd].fops_table= &rtc_table;
      cur_pcb->file_array[fd].inode=0;
      cur_pcb->file_array[fd].flags+=FD_FLAG_RTC;
      break;
    case 1:
      cur_pcb->file_array[fd].fops_table= &dir_table;
      cur_pcb->file_array[fd].inode=0;
      cur_pcb->file_array[fd].flags+=FD_FLAG_DIRECTORY;
      break;
    case 2:
      cur_pcb->file_array[fd].fops_table= &file_table;//&(*file_table[4]);//&(*file_table[4]);
      cur_pcb->file_array[fd].inode=entry.inode_num;
      cur_pcb->file_array[fd].flags+=FD_FLAG_FILE;
      break;
  }
  cur_pcb->file_array[fd].f_pos=0; //file position should start at beginning for all file types
  cur_pcb->file_arr_size+=1;
  cur_pcb->file_array[fd].fops_table->open(filename);
  //printf("Now set fd: %d to flags: %d\n",fd,cur_pcb->file_array[fd].flags);

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
  pcb_t* cur_pcb=getPCB(curr_process);
  int first_fd_ind = 2; //less than this is an invalid descriptor
  if (fd < first_fd_ind || fd >= MAX_OPEN_FILES){
    printf("fd=%d is invalid file_array index to close\n",fd);
    return -1;
  }
  cur_pcb->file_array[fd].flags=0;
//  printf("    fd:%d,flags now:%d\n",fd,cur_pcb->file_array[fd].flags);

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
