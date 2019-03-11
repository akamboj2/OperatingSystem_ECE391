#ifndef _IDT
#define _IDT

void idt_init();

void add_vector(int index, void *handler, int dpl, int type);

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
