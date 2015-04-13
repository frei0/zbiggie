#define ASM 1
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
	cmpl 	$10, %eax
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
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, set_pcb

syscall_fail:
	movl $-1, %eax				/*FAILURE*/
	ret


halt:
    pushl $0x48
    call putc 
    addl $4, %esp

    CALL get_current_pcb
    pushl 4(%eax)
    CALL switch_context #switch memory map
    CALL get_pcb
    movl (%eax), %esp #restore old stack

    pushl 48(%eax) #restore stored registers and iret context from pcb
    pushl 44(%eax)
    pushl 40(%eax)
    pushl 36(%eax)
    pushl 32(%eax)
    pushl 28(%eax)
    pushl 24(%eax)
    pushl 20(%eax)
    pushl 16(%eax)
    pushl 12(%eax)
    pushl 8(%eax)

    movl $0, %eax #return value for halt syscall

    popl %edi #restore registers (these came from the pcb)
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %ebp
    iret

execute: 
    CALL get_current_pcb
    addl $4, %esp #nuke ret addr
    popl 8(%eax) #move stored registers and iret context to pcb
    popl 12(%eax)
    popl 16(%eax)
    popl 20(%eax)
    popl 24(%eax)
    popl 28(%eax)
    popl 32(%eax)
    popl 36(%eax)
    popl 40(%eax)
    popl 44(%eax)
    popl 48(%eax)
    
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
set_pcb:
    pop %edx
    movl   (%esp), %ebx
    movl %ebx, 8(%eax)
    movl 4(%esp), %ebx
    movl %ebx, 12(%eax)
    movl 8(%esp), %ebx
    movl %ebx, 16(%eax)
    movl 12(%esp), %ebx
    movl %ebx, 20(%eax)
    movl 16(%esp), %ebx
    movl %ebx, 24(%eax)
    movl 20(%esp), %ebx
    movl %ebx, 28(%eax)
    movl 24(%esp), %ebx
    movl %ebx, 32(%eax)
    movl 28(%esp), %ebx
    movl %ebx, 36(%eax)
    movl 32(%esp), %ebx
    movl %ebx, 40(%eax)
    movl 36(%esp), %ebx
    movl %ebx, 44(%eax)
    movl 40(%esp), %ebx
    movl %ebx, 48(%eax)
    movl %esp, (%eax)
    movl $0, 4(%eax)
    push %edx
    ret
