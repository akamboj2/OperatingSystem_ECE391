#ifndef _IDT
#define _IDT

//initializating for idt
void idt_init();

//helper function to add vectors to idt
void add_vector(int index, void *handler, int dpl, int type);

//various handlers below
void divide_err();
void debug();
void nmi();
void breakpoint();
void overflow();
void bound();
void invalid_opcode();
void device_NA();
void double_fault();
void coprocessor_seg_overrun();
void invalid_tss();
void seg_not_present();
void stack_seg_fault();
void general_protection();
void page_fault();
void floating_point_err();
void alignment_check();
void machine_check();
void floating_point_exception();
void system_calls();
void rtc_interrupt();
void keyboard_interrupt();
#endif
