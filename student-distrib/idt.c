/*Interrupt desciptor table
  Useful references: ia-32 intel manual page 145, 156
*/
#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "assembly_linkage.h"
#include "i8259.h"


#define U_LVL 3
#define K_LVL 0
#define CODE_TYPE 1
#define DATA_TYPE 0
#define REG_C 0x0C
#define RTC_REG 0x70
#define RTC_RW 0x71
#define keyboard_port 0x60
#define char_upper 0x35
#define char_lower 0x01

volatile int curr_interrupt_vals_rtc [3] = {0, 0, 0};

void idt_init(){
  add_vector(0, divide_err, K_LVL, CODE_TYPE);

  add_vector(2, nmi, K_LVL, CODE_TYPE);
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

  add_vector(0x80, &system_calls, U_LVL, DATA_TYPE);

  add_vector(40, &rtc_interrupt, K_LVL, DATA_TYPE);
  add_vector(33, &keyboard_assembly, K_LVL, DATA_TYPE);
  //return;
}

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
  if(index == 0){  printf("  %x", idt[index].offset_31_16);  printf("%x  ", idt[index].offset_15_00);}

}

void divide_err(){
  while(1){printf("div by zero");}
}
void nmi(){
	 while(1){printf("NMI");}
}
void breakpoint(){
	 while(1){printf("breakpoint");}
}
void overflow(){
	 while(1){printf("overflow error");}
}
void bound(){
	 while(1){printf("out of bounds");}
}
void invalid_opcode(){
	 while(1){printf("invalid opcode");}
}
void device_NA(){
	 while(1){printf("device not available");}
}
void double_fault(){
	 while(1){printf("double fault");}
}
void coprocessor_seg_overrun(){
	 while(1){printf("coprecessor segment overrun");}
}
void invalid_tss(){
	 while(1){printf("invalid tss");}
}
void seg_not_present(){
	 while(1){printf("segment not present");}
}
void stack_seg_fault(){
	 while(1){printf("stack segment fault");}
}
void general_protection(){
	 while(1){printf("general protection");}
}
void page_fault(){
	 while(1){printf("page fault");}
}
void floating_point_err(){
	 while(1){printf("floating point error");}
}
void alignment_check(){
	 while(1){printf("alignment check");}
}
void machine_check(){
	 while(1){printf("machine check");}
}
void floating_point_exception(){
	 while(1){printf("floating point exception");}
}
void system_calls(){}
void rtc_interrupt(){
	printf("RTC triggered");
	//test_interrupts();
	cli();
	test_interrupts();
	outb(REG_C, RTC_REG);
	inb(RTC_RW);
	sti();
	send_eoi(8);
}
void keyboard_interrupt(){
	char char_list[53] = {'\0','1','2','3','4','5','6','7',
						'8','9','0','-','=','\0','\0','q','w',
						'e','r','t','y','u','i','o','p','[',']',
						'\0','\0','a','s','d','f','g','h','j',
						'k','l',';','\'','`','\0','\\','z',
						'x','c','v','b','n','m',',','.','/'};
	char c = inb(keyboard_port);
	if(c<=char_upper && c>=char_lower){
		char print_char = char_list[c-1];
		printf("%c", print_char);
	}

	send_eoi(1);
}
