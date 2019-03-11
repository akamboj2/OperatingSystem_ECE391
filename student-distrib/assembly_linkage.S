#define ASM     1
.globl keyboard_assembly

keyboard_assembly:
pushl %es
pushl %ds
pushl %edi
pushl %esi
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx
pushl %ebp

call keyboard_interrupt

popl %ebp
popl %edx
popl %ecx
popl %ebx
popl %eax
popl %esi
popl %edi
popl %ds
popl %es

iret


rtc_assembly:
pushl %es
pushl %ds
pushl %edi
pushl %esi
pushl %eax
pushl %ebx
pushl %ecx
pushl %edx
pushl %ebp

call rtc_interrupt

popl %ebp
popl %edx
popl %ecx
popl %ebx
popl %eax
popl %esi
popl %edi
popl %ds
popl %es

iret
