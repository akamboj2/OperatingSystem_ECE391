#include "sys_calls.h"
#include "types.h"
#include "lib.h"
#include "filesys.h"
#include "assembly_linkage.h"
#include "constants.h"
#include "paging.h"
#include "x86_desc.h"

//basic file tables
static ftable file_table = {&file_read, &file_write, &file_open, &file_close};
static ftable dir_table = {&dir_read, &dir_write, &dir_open, &dir_close};
static ftable rtc_table = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};

//first two file structs in the file array
int32_t stdin_write(int32_t a, const void* b, int32_t c){return 0;};
int32_t stdout_read(int32_t a,void* b,int32_t c){return 0;};
static ftable stdin_table = {&terminal_read,&stdin_write,&terminal_open,&terminal_close};
static ftable stdout_table = {&stdout_read,&terminal_write,&terminal_open,&terminal_close};

 //curr_process = 0;
int32_t user_vid_mem=0;
int32_t process_count = 0;
int32_t highest_terminal_processes[3] = {0,0,0};
int32_t pcb_slots[6] = {0,0,0,0,0,0};
int32_t process_per_terminal[3] = {0,0,0};


/*getPCB
 * Description: Gets curr process' pcb
 * Inputs: number of curr process
 * Outputs/Return Values: return pointer to curr process' pcb
 * Side Effects: none
 */
pcb_t * getPCB(int32_t curr){
    return (pcb_t*)(_8MB - (curr)*_8KB);
}


/*halt
 * Description: jump to execute's return for the parent function
 * Inputs: return value to be transmitted to execute via eax register
 * Outputs/Return Values: Returns 0 (never actually returns! jumps to exectute's return)
 * Side Effects: writes to eax, esp, and ebp
 */
int32_t halt (uint8_t status){
  //close files in fd
  int i, j;
  cli();
  process_count--;
  process_per_terminal[curr_terminal-1]--;
  pcb_t* pcb_to_be_halted = getPCB(highest_terminal_processes[curr_terminal-1]);

  //correct file array. this will close any files opened by the process
  for(i=0; i<MAX_OPEN_FILES; i++){
      if(pcb_to_be_halted->file_array[i].flags != 0)
          pcb_to_be_halted->file_array[i].fops_table->close(i);
      pcb_to_be_halted->file_array[i].flags = 0;
  }

  for(j = 0; j < MAX_ARGS; j++){
    pcb_to_be_halted->args[j] = '\0';
  }

  pcb_slots[highest_terminal_processes[curr_terminal-1]-1] = 0;

  //restore parent paging
  if(pcb_to_be_halted->parent_task != NULL){
    pageDirectory[_4B] = (_8MB + (((pcb_to_be_halted->parent_task->process_num)-1)*_4MB)) | MAP_MASK;
    highest_terminal_processes[curr_terminal-1] = pcb_to_be_halted->parent_task->process_num;
    tss.esp0 = pcb_to_be_halted->parent_task->esp0;
  }
  else{
    highest_terminal_processes[curr_terminal-1] = 0;
    clear();
    set_cursors(0,0);			//reset cursor
    update_cursor(0,0);
    execute((const uint8_t*)("shell"));
    //return 0;
  }

  //flush tlb
  cli();
  asm volatile ("MOVL %CR3, %eax;");
  asm volatile ("MOVL %eax, %CR3;");
  sti();



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
  int k = 0;
  int args_flag = 0;
  //command = "fish";
  //printf("%c", *command);

  if(!command){
    return -1;
  }
  if(process_count >= 6 /*|| process_per_terminal[curr_terminal-1] >= 4*/){
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

  process_count++;
  process_per_terminal[curr_terminal-1]++;

  int pcb_index = 0;
  while(pcb_index < 6){
    if(pcb_slots[pcb_index] == 0)
      break;
    pcb_index++;
  }
  /*for(pcb_index = 0; pcb_index < 6; pcb_index++){
    if(pcb_slots[pcb_index] == 0)
      break;
  }*/
  pcb_slots[pcb_index] = 1;

  //PCB initialization
  pcb_t * pcb = (pcb_t *)(_8MB - (pcb_index+1)*_8KB);

  //skips leading spcaces before args
  while(command[i] == ' ') {
    i++;
  }

  //writes args into a buffer in the pcb struct
  int j = 0;
  while(command[i] != '\0') {
    pcb->args[j] = command[i];
    //printf("%c",argBuffer[j]);
    i++;
    j++;
  }

  //Paging
  pageDirectory[_4B] = (_8MB + (pcb_index*_4MB)) | MAP_MASK;
  //4B bc it's 32th entry in page directory (director is size 4MB) 32*4 =128MB address

  //flush tlb
  cli();
  asm volatile ("MOVL %CR3, %eax;");
  asm volatile ("MOVL %eax, %CR3;");
  sti();

  uint32_t * eip = (uint32_t*)&(data[24]); //eip holds ptr to 128MB

  //load file into program page
  int32_t nbytes = file_size(temp.inode_num);

  //copies data from executable file to virtual memory starting at PROG_LOAD_ADDR
  if(read_data(temp.inode_num, 0, (uint8_t *)PROG_LOAD_ADDR, nbytes) == -1)
    return -1;


  if(highest_terminal_processes[curr_terminal-1] != 0){
    pcb->parent_task =  (pcb_t *)(_8MB - (highest_terminal_processes[curr_terminal-1])*_8KB);
  }
  else pcb->parent_task = NULL;

  highest_terminal_processes[curr_terminal-1] = pcb_index + 1;

  //Set up the PCB
  pcb->process_num = pcb_index+1;
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
  pcb->file_arr_size = STDI_O;

  for(k = STDI_O; k < MAX_OPEN_FILES; k++){
    pcb->file_array[k].flags = 0;
  }





  //Context Switch
  tss.ss0 = KERNEL_DS;
  tss.esp0 = _8MB - (pcb_index)*_8KB - _ONE_STACK_ENTRY; //_ONE_STACK_ENTRY used to go to bottom of kernel stack for curr process
  pcb->esp0 = tss.esp0;

  //curr_process++;
  //for eip argument, push eip found above

  context_switch((uint32_t*)*eip);

  //asm volatile ("iret;");

  return 0;
}


/*read
 * Description: Reads file into buf
 * Inputs: file number, buf to read into, and num bytes to read
 * Outputs/Return Values: Returns position in file
 * Side Effects: none
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
  pcb_t* cur_pcb=getPCB(  highest_terminal_processes[curr_terminal-1]);
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0 || fd == 1 || cur_pcb->file_array[fd].flags == 0)
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
 * Description: Writes data in buf to screen
 * Inputs: file number, buf with file data, num bytes to write
 * Outputs/Return Values: position in file
 * Side Effects: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
  pcb_t* cur_pcb=getPCB(  highest_terminal_processes[curr_terminal-1]);
  if(buf == NULL || fd >= MAX_OPEN_FILES || fd < 0 || fd == 0 || cur_pcb->file_array[fd].flags == 0)
    return -1;

  if(cur_pcb->file_array[fd].flags & FD_FLAG_FILE)
    return -1;

  //writes data from buffer to termianl
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
  pcb_t* cur_pcb=getPCB(  highest_terminal_processes[curr_terminal-1]);
  //printf("cur_pcb->file_arr_size: %d\n",cur_pcb->file_arr_size);
  if(read_dentry_by_name(filename, &entry) || cur_pcb->file_arr_size>=MAX_OPEN_FILES){ //find dentry and return -1 if it fails
    return -1;
  }
  //printf("in open\n");
  //now find empty entry in file_array
  int fd = STDI_O; //this is loop counter and also holds empty index in file_array
  for(fd = STDI_O;fd < MAX_OPEN_FILES;fd++){
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
  pcb_t* cur_pcb=getPCB(  highest_terminal_processes[curr_terminal-1]);
  int first_fd_ind = STDI_O; //less than this is an invalid descriptor
  if ((fd < first_fd_ind || fd >= MAX_OPEN_FILES) || cur_pcb->file_array[fd].flags == 0){
    //printf("fd=%d is invalid file_array index to close\n",fd);
    return -1;
  }
  cur_pcb->file_arr_size--;
  cur_pcb->file_array[fd].flags=0;
//  printf("    fd:%d,flags now:%d\n",fd,cur_pcb->file_array[fd].flags);

  return 0;
}

/*getargs
 * Description: Gets the args of the curr process
 * Inputs: buf to write into and number of bytes to write
 * Outputs/Return Values: Return 0
 * Side Effects: none
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
  pcb_t* pcb = getPCB(  highest_terminal_processes[curr_terminal-1]);
   if(pcb->args == NULL)
     return -1;

   uint32_t character = 0;
   while(pcb->args[character] != NULL) {
     character++;
   }

   if(character > (nbytes - 1))
     return -1;

   memcpy(buf, pcb->args, nbytes);    //copies args into buffer

   return 0;

}

/*vidmap
 * Description: set user's pointer to point to a page in virtual memory that points
              to the actual video memory in the page table
 * Inputs: pointer to pointer of where vid mem should be
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t vidmap (uint8_t** screen_start){

  //first check if valid pointer (not null and within current process memory)
  uint32_t prog_start=_128MB, prog_end= USER_VID_ADDR;
  if (screen_start==NULL || (int32_t)screen_start<prog_start || (int32_t)screen_start>=prog_end){
    //printf("Invalid pointer address 0x%x.\n",screen_start);
    return -1;
  }

  //int32_t vid_page= VID_PAGE; //this is now in syscall.h

  //set page directory to correct physical address
  //below this should be the index USER_VID_ADDR/_4MB = 33
  pageDirectory[USER_VID_ADDR/_4MB]= (int32_t)pgTbl_vidMem | VID_PAGE; //directory bits should be same as page's--4kb page size

  //set page table to correct physical address
  pgTbl_vidMem[0]= VIDEO | VID_PAGE;

  //set the global variable user_vid_mem (virtual address mapped to same phsyical video memory)
  //you only need to set it once (same for all processes)
  if (user_vid_mem==0){
    user_vid_mem = ((int32_t)pgTbl_vidMem & VID_MASK) | USER_VID_ADDR;
  }
  //shouldn't it be user_vid_mem = (33<<22) | 0 | 0
  // bc it's 33th page directory, and 0th table with 0th offset?

  //note to test fish: break at vidmap, check what it's being set to
  //break at rtc, see if pgTbl_vidMem is pointing to correct thing or set to correct thing
  //switch terminals and then break in rtc until u on correct background curr_scheduled to see if the background process is still running with correct vidme
  *screen_start= (uint8_t*)user_vid_mem;

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
