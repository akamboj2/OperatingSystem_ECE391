/*Interrupt desciptor table
  Useful references: ia-32 intel manual page 145, 156
*/
#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "assembly_linkage.h"
#include "i8259.h"
#include "keyboard.h"
#include "rtc.h"
#include "scheduler.h"


#define U_LVL 3
#define K_LVL 0
#define CODE_TYPE 1
#define DATA_TYPE 0
#define REG_C 0x0C
#define RTC_REG 0x70
#define RTC_RW 0x71


/*idt_init
* adds vector to the the idt
* modifies the idt
*/
void idt_init(){
  add_vector(0, &divide_err, K_LVL, CODE_TYPE);
  add_vector(1, &debug, K_LVL, CODE_TYPE);
  add_vector(2, &nmi, K_LVL, CODE_TYPE);
  add_vector(3, &breakpoint, U_LVL, DATA_TYPE);
  add_vector(4, &overflow, U_LVL, DATA_TYPE);
  add_vector(5, &bound, U_LVL, DATA_TYPE);
  add_vector(6, &invalid_opcode, K_LVL, CODE_TYPE);
  add_vector(7, &device_NA, K_LVL, CODE_TYPE);
  add_vector(8, &double_fault, K_LVL, CODE_TYPE);
  add_vector(9, &coprocessor_seg_overrun, K_LVL, CODE_TYPE);
  add_vector(10, &invalid_tss, K_LVL, CODE_TYPE);
  add_vector(11, &seg_not_present, K_LVL, CODE_TYPE);
  add_vector(12, &stack_seg_fault, K_LVL, CODE_TYPE);
  add_vector(13, &general_protection, K_LVL, CODE_TYPE);
  add_vector(14, &page_fault, K_LVL, CODE_TYPE);

  add_vector(16, &floating_point_err, K_LVL, CODE_TYPE);
  add_vector(17, &alignment_check, K_LVL, CODE_TYPE);
  add_vector(18, &machine_check, K_LVL, CODE_TYPE);
  add_vector(19, &floating_point_exception, K_LVL, CODE_TYPE);

  add_vector(0x80, &system_calls_assembly, U_LVL, 1);

  add_vector(40, &rtc_assembly, K_LVL, DATA_TYPE);
  add_vector(33, &keyboard_assembly, K_LVL, DATA_TYPE);
  add_vector(32, &pit_assembly, K_LVL,DATA_TYPE);
  //return;
}

/*add_vector
*index - where in table to add vector
*handler - ptr to the interrupt handler of that vector
*privilege - sets whether user or kernel level 3 or 0 respectively
*type - determines if a trap or sys call
*/
void add_vector(int index, void *handler, int privilege, int type){
  //set handler idt[index]

  SET_IDT_ENTRY(idt[index], handler);
  idt[index].seg_selector = KERNEL_CS;
  idt[index].present = 1;
  idt[index].dpl = privilege;
  idt[index].size = 1;
  idt[index].reserved0 = 0;
  idt[index].reserved1 = 1;
  idt[index].reserved2 = 1;
  idt[index].reserved3 = type;
  idt[index].reserved4 = 0;
  //if(index == 0){  printf("  %x", idt[index].offset_31_16);  printf("%x  ", idt[index].offset_15_00);}

}

/*
*ALL interrupt handler functions below
*/
void divide_err(){
  printf2("div by zero");
  cli();
  while(1){};
}
void debug(){
  printf2("debug");
  cli();
  while(1){};
}

void nmi(){
	printf2("NMI");
  cli();
  while(1){};
}
void breakpoint(){
	 printf2("breakpoint");
   cli();
   while(1){};
}
void overflow(){
	 printf2("overflow error");
   cli();
   while(1){};
}
void bound(){
	 printf2("out of bounds");
   cli();
   while(1){};
}
void invalid_opcode(){
	printf2("invalid opcode");
  cli();
  while(1){};
}
void device_NA(){
	 printf2("device not available");
   cli();
   while(1){};
}
void double_fault(){
	 printf2("double fault");
   cli();
   while(1){};
}
void coprocessor_seg_overrun(){
	printf2("coprecessor segment overrun");
  cli();
  while(1){};
}
void invalid_tss(){
	printf2("invalid tss");
  cli();
  while(1){};
}
void seg_not_present(){
	 printf2("segment not present");
   cli();
   while(1){};
}
void stack_seg_fault(){
	 printf2("stack segment fault");
   cli();
   while(1){};
}
void general_protection(){
	 printf2("general protection");
   cli();
   while(1){};
}
void page_fault(){
	 printf2("page fault");
   cli();
   while(1){};
}
void floating_point_err(){
	 printf2("floating point error");
   cli();
   while(1){};
}
void alignment_check(){
	 printf2("alignment check");
   cli();
   while(1){};
}
void machine_check(){
	 printf2("machine check");
   cli();
   while(1){};
}
void floating_point_exception(){
	 printf2("floating point exception");
   cli();
   while(1){};
}
//void system_calls(){} system call jumpts to assembly and then it's own file

/*rtc_interrupt
*makes sure to clean the status register (regitser C)
* sends eoi to signify done with interrupt
*/
void rtc_interrupt(){
  cli();
  rtc_handler();
  sti();
}

/*keyboard_interrupt
* reads the character written by checking the keyboard port register (0x60)
* then maps that value to the character table and prints the resulting charcater
* to the screen.
* sends eoi to signify done with interrupt
*/
void keyboard_interrupt(){
	keyboard_handler();
}

//NOTE: PIT Interrupt assembly calls handler directly
