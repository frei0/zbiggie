#define ASM 1
#include "syscall.h" 

.globl syscall
.align 4



syscall: 
    pushal
	decl	%eax	
	cmpl 	$9, %eax
	ja		syscall_fail
	call *jump_table(,%eax,4)
    popal
    iret

jump_table: 	#jump table of syscalls
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

syscall_fail:
	movl $-1, %eax				#FAILURE
	ret


halt:
    pushl $0x48
    call putc 
    addl $4, %esp
    jmp halt_ret_label

execute: 
    pushl $0x45
    call putc 
    addl $4, %esp
    CALL load_exec_to_mem
    #pushl halt
    jmp %eax
halt_ret_label:
    ret

read: 
    pushl $0x52
    call putc 
    addl $4, %esp
    ret

write: 
    pushl $0x57
    call putc 
    addl $4, %esp
    ret

open: 
    ret

close:
    ret

getargs: 
    ret

vidmap:
    ret
 
set_handler:
    ret

sigreturn: 
    ret
