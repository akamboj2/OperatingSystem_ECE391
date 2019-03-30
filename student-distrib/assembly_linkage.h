#ifndef _assembly_h
#define _assembly_h

#ifndef ASM
#include "sys_calls.h"
#include "idt.h"
void keyboard_assembly();
void rtc_assembly();
void system_calls_assembly();
#endif
#endif
