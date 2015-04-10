#define ASM 1
#include "syscall.h" 

.globl syscall
.align 4



syscall: 
	decl	%eax	
	cmpl 	$9, %eax
	ja		syscall_fail
	jmp 	*jump_table(,%eax,4)
	

jump_table: 	#jump table of syscalls
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

syscall_fail:
	movl $-1, %eax				#FAILURE
	ret


halt:
    pushal
    pushl $0x48
    call putc 
    addl $4, %esp
    popal
    iret

execute: 
    pushal
    pushl $0x45
    call putc 
    addl $4, %esp
    CALL load_exec_to_mem
    #pushl halt
    jmp %eax
    popal
    iret

read: 
    pushal
    pushl $0x52
    call putc 
    addl $4, %esp
    popal
    iret

write: 
    pushal
    pushl $0x57
    call putc 
    addl $4, %esp
    popal
    iret

open: 
    pushal
    popal
    iret

close:
    pushal
    popal
    iret

getargs: 
    pushal
    popal
    iret

vidmap:
    pushal
    popal
    iret
 
set_handler:
    pushal
    popal
    iret

sigreturn: 
    pushal
    popal
    iret
