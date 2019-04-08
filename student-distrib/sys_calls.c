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
ftable * stdin_table = {&terminal_read,&stdin_write,&terminal_open,&terminal_close};
ftable * stdout_table = {&stdout_read,&terminal_write,&terminal_open,&terminal_close};


int32_t curr_process = 0;


/*halt
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t halt (uint8_t status){
  //close files in fd
  //restore parent data (esp and ebp for parent?)

  //restore parent paging
  curr_process--;
  pageDirectory[32] = ((2*_4MB) + (curr_process*_4MB)) | MAP_MASK;

  tss.esp0 = 2*_4MB - (curr_process)*2*PAGE_MEM_SIZE - 4;

  //to mimic a return in exec, store ebp into pcb before iret. Then take ebp in pcb and load into %ebp register. Then iret


  //printf("IN HALT!\n");
  return 0;
}

/*execute
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
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
  uint8_t filename[32] = {'\0'};
  uint8_t data[32] = {'\0'};

  //printf(temp.inode_num);

  while(command[i] != '\0' && command[i] != ' ' && i<32){
    filename[i] = command[i];
    i++;
  }

  dentry_t temp;
  if(read_dentry_by_name(filename, &temp) == -1)
    return -1;

  if(command[i] != ' ')
    args_flag = 1;

  file_open(filename);
  if(file_read(0,data,32) == -1){
      file_close(0);
      return -1;
  }
  file_close(0);

  if(data[0] != 127 || data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
    return -1;

  //Paging
  pageDirectory[32] = (_8MB + (curr_process*_4MB)) | MAP_MASK;

  //flush tlb
  cli();
  asm volatile ("MOVL %CR3, %eax;");
  asm volatile ("MOVL %eax, %CR3;");
  sti();

  uint32_t * eip = (int32_t*)&(data[24]); //eip holds ptr to 128MB
  //printf("%d\n", eip);

  //printf("%d", temp->inode_num);
  //load file into program page
  int32_t nbytes = file_size(temp.inode_num);
  //printf("%d", nbytes);
  //FIX BEFORE 6 PM PLS
  if(read_data(temp.inode_num, 0, PROG_LOAD_ADDR, nbytes) == -1)
    return -1;



  //PCB code
  pcb_t * pcb = _8MB - (curr_process+1)*_8KB;
  if(curr_process != 0){
    pcb->parent_task =  _8MB - (curr_process)*_8KB;
  }
  else pcb->parent_task = NULL;
  curr_process++;

  pcb->process_num = curr_process;
  pcb->eip = *eip;

  pcb->file_array[0].fops_table = stdin_table;
  pcb->file_array[1].fops_table = stdout_table;
  pcb->file_array[0].inode = -1;
  pcb->file_array[1].inode = -1;
  pcb->file_array[0].f_pos = 0;
  pcb->file_array[1].f_pos = 0;
  pcb->file_array[0].flags = FD_FLAG_PRESENT;
  pcb->file_array[1].flags = FD_FLAG_PRESENT;
  pcb->file_arr_size = 2;

  //Context Switch
  tss.ss0 = KERNEL_DS;
  tss.esp0 = _8MB - (curr_process-1)*_8KB - _ONE_STACK_ENTRY; //_ONE_STACK_ENTRY used to go to bottom of kernel stack for curr process
  //printf("%x",get_eip());
  //for eip argument, push eip found above
  //cli();
  context_switch(*eip);
  //sti();
  /*asm volatile ("mov $35, %ax;");
  asm volatile ("mov %ax, %ds;");
  asm volatile ("mov %ax, %es;");
  asm volatile ("mov %ax, %fs;");
  asm volatile ("mov %ax, %gs;");

  asm volatile ("movl %esp, %eax;");
  asm volatile ("pushl $35;");  //#ds
  asm volatile ("pushl (0x8400000-0x4);"); //#esp points to 132MB minus one block up
  asm volatile ("pushfl;");  //#push eflags
  asm volatile ("pushl $27;");  //#cs
  asm volatile ("pushl get_eip;"); //#push bits[24:27]
  printf("%x\n", get_eip());
  asm volatile ("iret;");*/

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
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t open (const uint8_t* filename){
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
