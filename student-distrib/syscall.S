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
    pushl $0x48
    call putc 
    addl $4, %esp

    CALL get_current_pcb
    pushl 4(%eax)
    CALL switch_context 
    CALL get_pcb
    movl (%eax), %esp

    pushl 48(%eax)
    pushl 44(%eax)
    pushl 40(%eax)
    pushl 36(%eax)
    pushl 32(%eax)
    pushl 28(%eax)
    pushl 24(%eax)
    pushl 20(%eax)
    pushl 16(%eax)
    pushl 12(%eax)

    movl $0, %eax
    #addl $4, %esp

    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %ebp
    iret

execute: 
    CALL get_current_pcb
    popl 8(%eax)
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

    pushl %ebx
    CALL load_exec_to_mem
    addl $4, %esp
    cmpl $-1, %eax
    je syscall_fail

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

