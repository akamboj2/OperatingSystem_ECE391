#define ASM     1
.globl keyboard_assembly
.globl rtc_assembly
.globl system_calls_assembly
.globl context_switch

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


system_calls_assembly:
pushl %es
pushl %ds
pushl %edi
pushl %esi
pushl %ebp

pushl %edx
pushl %ecx
pushl %ebx

cmpl $1,%eax
jl invalid_sys_call
cmpl $10,%eax
jg invalid_sys_call

call *sys_call_table(,%eax,4) #multiply by 4 bc each address is 4 bytes

invalid_sys_call:
popl %ebx
popl %ecx
popl %edx

popl %ebp
popl %esi
popl %edi
popl %ds
popl %es
iret

#credit to OSDev
context_switch:
#push onto stack for IRET in sys call functions to remove
cli
movl %eax, %ebx

movw $0x2B, %ax
movw %ax, %ds
movw %ax, %es
movw %ax, %fs
movw %ax, %gs

movl %esp, %eax
pushl $0x2B  #ds
pushl $(0x83FFFFC) #esp points to 132MB minus one block up
pushfl  #push eflags
popl  %edx
orl   $0x200, %edx #enables interrupt bit in eflags for user mode since cli disables it
pushl %edx
pushl $0x23  #cs
pushl %ebx #push bits[24:27]
#sti
iret

.global reverse_system_call
reverse_system_call:
leave
ret



sys_call_table:
.long 0 #sys_calls are from 1-10 so need to exclude zero?
.long halt
.long execute
.long read
.long write
.long open
.long close
.long getargs
.long vidmap
.long set_handler
.long sigreturn
