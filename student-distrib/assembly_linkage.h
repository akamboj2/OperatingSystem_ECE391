#ifndef _assembly_h
#define _assembly_h

#ifndef ASM
#include "sys_calls.h"
#include "idt.h"
void keyboard_assembly();
void rtc_assembly();
void pit_assembly();

void system_calls_assembly();
void context_switch(uint32_t* eip);
#endif
#endif
