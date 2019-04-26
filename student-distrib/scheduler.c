#include "scheduler.h"
#include "idt.h"
#include "lib.h"
#include "i8259.h"
#include "types.h"

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
