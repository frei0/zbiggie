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

jump_table: 	/*jump table of syscalls*/
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

syscall_fail:
	movl $-1, %eax				/*FAILURE*/
	ret


halt:
    pushl $0x48
    call putc 
    addl $4, %esp

    CALL get_current_pcb
    pushl 4(%eax)
    CALL get_pcb
    movl (%eax), %esp
    ret

execute: 
    CALL get_current_pcb
    movl %esp, (%eax)

    CALL setup_new_process

    pushl %ebx
    CALL load_exec_to_mem
    addl $4, %esp

    /*pushl halt*/
    /* jmp %eax asdf */
    pushl $USER_DS
    movl  $LOAD_ADDR,%ebx
    subl  $4,%ebx
    pushl %ebx
    pushf
    orl   $0x200,(%esp)
    pushl $USER_CS
    pushl %eax
    iret

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
