#define ASM 1
#define PARENT 4
#include "syscall.h" 

.globl syscall
.align 4



syscall: 
    pushl %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi

	decl	%eax	
	cmpl 	$9, %eax
	ja		syscall_fail
	call *jump_table(,%eax,4)

    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %ebp

    iret

jump_table: 	/*jump table of syscalls*/
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
syscall_fail:
	movl $-1, %eax				/*FAILURE*/
	ret


halt:
    CALL free_current_pcb
    CALL get_current_pcb
    pushl PARENT(%eax)
    CALL switch_context #switch memory map
    CALL get_pcb
    movl (%eax), %esp #restore old stack

    movl %ebx, %eax #return value for halt syscall

    ret

execute: 
    pushl %ebx
    CALL exec_check
    addl $4, %esp
    cmpl $0, %eax
    je syscall_fail 

    CALL get_current_pcb

    movl %esp, (%eax)

    pushl %ebx #push string from user to load executable to memory
    CALL load_exec_to_mem
    addl $4, %esp
    cmpl $-1, %eax #fail if unable to load
    je syscall_fail

    pushl $USER_DS #push iret context to jump to user executable
    movl  $LOAD_ADDR,%ebx
    subl  $4,%ebx
    pushl %ebx
    pushf
    orl   $0x200,(%esp)
    pushl $USER_CS
    pushl %eax
    iret

read: 

    pushl %edx
    pushl %ecx

    
    pushl %ebx
    call get_file
    addl $4, %esp

    pushl %eax
    #sti
    call kread

    addl $12, %esp
    ret

write: 
    pushl %edx
    pushl %ecx

    
    pushl %ebx
    call get_file
    addl $4, %esp

    pushl %eax
    call kwrite

    addl $12, %esp
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
