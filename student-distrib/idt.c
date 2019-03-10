/*Interrupt desciptor table
  Useful references: ia-32 intel manual page 145, 156
*/

#include "x86_desc.h"
#include "idt.h"


#define U_LVL 3
#define K_LVL 0
#define CODE_TYPE 1
#define DATA_TYPE 0

void idt_init(){

  add_vector(0, &divide_err, K_LVL, CODE_TYPE);

  add_vector(2, &nmi, K_LVL, CODE_TYPE);
  add_vector(3, &breakpoint, U_LVL, DATA_TYPE);
  add_vector(4, &overflow, U_LVL, DATA_TYPE);
  add_vector(5, &bound, U_LVL, DATA_TYPE);
  add_vector(6, &invalid_opcode, K_LVL, CODE_TYPE);
  add_vector(7, &device_NA, K_LVL, CODE_TYPE);
  add_vector(8, &double_fault, K_LVL, CODE_TYPE);
  add_vector(9, &coproessor_seg_overrun, K_LVL, CODE_TYPE);
  add_vector(10, &invalid_tss, K_LVL, CODE_TYPE);
  add_vector(11, &seg_not_present, K_LVL, CODE_TYPE);
  add_vector(12, &stack_seg_fault, K_LVL, CODE_TYPE);
  add_vector(13, &general_protection, K_LVL, CODE_TYPE);
  add_vector(14, &page_fault, K_LVL, CODE_TYPE);

  add_vector(16, &floating_point_err, K_LVL, CODE_TYPE);
  add_vector(17, &alignment_check, K_LVL, CODE_TYPE);
  add_vector(18, &machine_check, K_LVL, CODE_TYPE);
  add_vector(19, &floating_point_exception, K_LVL, CODE_TYPE);

  add_vector(0x80, &system_calls, U_LVL, DATA_TYPE);

  add_vector(32, &rtc_interrupt, K_LVL, DATA_TYPE);
  add_vector(33, &keyboard_interrupt, K_LVL, DATA_TYPE);

}

void add_vector(int index, void *handler, int privilege, int type){
  //set handler idt[index]
  idt[index].offset_15_00 = (uint32_t)(handler) & 0xFFFF;
  idt[index].offset_31_16 = ((uint32_t)(handler) & 0xFFFF0000) >> 16;
  idt[index].seg_selector = KERNEL_CS;
  idt[index].present = 1;
  idt[index].dpl = privilege;
  idt[index].size = 1;
  idt[index].reserved0 = 0;
  idt[index].reserved1 = 1;
  idt[index].reserved2 = 1;
  idt[index].reserved3 = type;
  idt[index].reserved4 = 0;
}

void divide_err(){}
void nmi(){}
void breakpoint(){}
void overflow(){}
void bound(){}
void invalid_opcode(){}
void device_NA(){}
void double_fault(){}
void coproessor_seg_overrun(){}
void invalid_tss(){}
void seg_not_present(){}
void stack_seg_fault(){}
void general_protection(){}
void page_fault(){}
void floating_point_err(){}
void alignment_check(){}
void machine_check(){}
void floating_point_exception(){}
void system_calls(){}
void rtc_interrupt(){}
void keyboard_interrupt(){}
