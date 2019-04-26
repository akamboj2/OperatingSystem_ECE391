#include "scheduler.h"
#include "idt.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "constants.h"
#include "sys_calls.h"
#include "paging.h"
#include "x86_desc.h"


int curr_sheduled=1; //either 1,2 or 3 (not 0)

/*pit handler
 *
 * Inputs: None
 * Outputs: executes at pit interrupt, does scheduling!
 * Side Effects: None
 */
void pit_handler(){
  printf("PIT INTERRUPT!\n");
  /* form lecture slides on scheduling:
  Utilize the kernel stack (think about what you did for HALT)
  u Again you will be using assembly to do the context switch
  u Switch ESP/EBP to next process’ kernel stack
  u Restore next process’ TSS
  u Flush TLB on process switch

  u Save esp/ebp
  u Switch process paging
  u Set TSS
  u Update running video coordinates
  u Restore next process’ esp/ebp
  */
int next_scheduled = curr_sheduled%3+1; //mod 3 first bc we are rotating between 1,2,3 (excluding 0)

 //update paging (same page directory and table, just remap video memory accordingly)
 if (next_scheduled==curr_terminal){
  //if the next one is the same as the one we are displaying, make virtual video mem point to actual video memory
  pgTbl_vidMem[0]=VIDEO|VID_PAGE; //this is so that the user (calling vidmap syscall) can print
  video_mem = (char *)VIDEO;  //this is so that printf/puts/putc (which dereferences vid_mem) can print
 }else{
   //otherwise we need to update it to point to the correct video buffer in physical memory
   switch(next_scheduled){
    case 1:
      pgTbl_vidMem[0]=VIDEO1|VID_PAGE;
      video_mem = (char *)VIDEO1;
      break;
    case 2:
      pgTbl_vidMem[0]=VIDEO2|VID_PAGE;
      video_mem = (char *)VIDEO2;
      break;
    case 3:
      pgTbl_vidMem[0]=VIDEO3|VID_PAGE;
      video_mem = (char *)VIDEO3;
      break;
   }
 }
//update program image in virtual to point to correct physical
pageDirectory[_4B] = (_8MB + ((highest_terminal_processes[next_scheduled-1]-1)*_4MB)) | MAP_MASK;

pcb_t* pcb_curr_process = getPCB(highest_terminal_processes[curr_sheduled-1]);
pcb_curr_process->esp0 = tss.esp0;

pcb_t* pcb_to_be_scheduled = getPCB(highest_terminal_processes[next_scheduled-1]);

 //update tss for next process
//flush tlb
cli();
asm volatile ("MOVL %CR3, %eax;");
asm volatile ("MOVL %eax, %CR3;");
sti();

tss.esp0 = _8MB - ((pcb_to_be_scheduled->process_num)-1)*_8KB - _ONE_STACK_ENTRY;

sti();

//prepare for jump to execute by finding the correct esp value in parent pcb
/*asm volatile(
  ""
  "MOVL %0, %%eax;"
  "MOVL %1, %%esp;"
  "MOVL %2, %%ebp;"
  "JMP reverse_system_call;"
  : : "r" ((uint32_t)status), "r" (pcb_to_be_scheduled->esp), "r" (pcb_to_be_scheduled->ebp) : "eax"
);*/

  send_eoi(0);

}

/*pit handler
 * Inputs: None
 * Outputs: none
 * Side Effects: called during kernel.c within cli sti critical section! so no interrupts should be in here!
 * Function: initializes PIT setup/mode and sets pit frequency (just change freq variable)
 */
void pit_init(){
  //see https://wiki.osdev.org/PIT for specifications
  //see https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interval_Timer for example code of setting 100 Hz freq
  enable_irq(0);      //enable pit or interval timer
  int8_t pit_status= 0x36, //channel bits (6-7), access mode lobyte/hibyte (bits4-5), mode 3 square wave generator (bits 1-3), bit 0 binary mode
     out_highB,out_lowB;
    //to make the PIT fire at a certain frequency f, you need to figure out an integer x, such that 1193182 / x = f.
    //If a frequency of 100 hz is desired, we see that the necessary divisor is 1193182 / 100 = 11931
  int32_t  freq = 100, numerator=1193182;
  int16_t out_val;
  out_val = numerator/freq;
  out_highB= (out_val & 0xFF00)>>8;
  out_lowB= out_val&0xFF;
  outb(PIT_REG_MODE,pit_status);
  /* "For the "lobyte/hibyte" mode, 16 bits are always transferred as a pair, with the
  * lowest 8 bits followed by the highest 8 bits (both 8 bit transfers are to the same IO port,
  * sequentially -- a word transfer will not work)."
  */
  outb(PIT_CHAN0,out_lowB);
  outb(PIT_CHAN0,out_highB);
}
