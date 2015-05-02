#define ASM 1
#define PARENT 4
#include "syscall.h" 
#include "tasking_funcs.h"

.globl syscall,kexecute
.align 4



syscall: 
    pushl %ebp
    pushl %ebx
    pushl %ecx
    pushl %edx
    pushl %esi
    pushl %edi

	decl	%eax	
	cmpl 	$SYSCALL_MAX_INDEX, %eax
	ja	    invalid_syscall	
    cmpl    $0, %eax
    jb      invalid_syscall 
    
	call *jump_table(,%eax,4)
unsave:
    popl %edi
    popl %esi
    popl %edx
    popl %ecx
    popl %ebx
    popl %ebp

    iret

jump_table: 	/*jump table of syscalls*/
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
invalid_syscall:
    movl $-1,%eax
    jmp unsave
syscall_fail:
    movl $-1,%eax
    ret
launch_new_shell:
    movl execstring,%ebx
    jmp kexecute


halt:
    CALL free_current_pcb
    CALL get_current_pid
    cmpl $4, %eax
    jb launch_new_shell 
    CALL get_current_pcb
    pushl PARENT(%eax)
    CALL switch_context #switch memory map
    CALL get_pcb
    movl (%eax), %esp #restore old stack

    movl %ebx, %eax #return value for halt syscall

    ret

execute:
    pushl %ebx
    CALL strlen
    pushl %eax
    pushl %ebx
    CALL usr_ptr_ok
    addl $12, %esp
    orl %eax,%eax
    jz syscall_fail
kexecute:
    CALL get_current_pcb

    movl %eax, %edi

    pushl %ebx #push string from user to load executable to memory
    CALL load_exec_to_mem
    addl $4, %esp #pop c-args
    cmpl $-1, %eax #fail if unable to load
    je syscall_fail
    cmpl $1, %eax #fail if unable to load
    je syscall_fail

    movl %esp, (%edi)

    pushl $USER_DS #push iret context to jump to user executable
    movl  $LOAD_ADDR,%ebx
    subl  $4,%ebx
    pushl %ebx
    pushf
    orl   $EFLAG_INT_ENABLE,(%esp) //change eflags to enable interrupts after jump to user
    pushl $USER_CS
    pushl %eax
    iret
read: 

    pushl %edx #save edx, ecx
    pushl %ecx
    call usr_ptr_ok
    orl %eax,%eax
    jz read_bad_ptr

    
    pushl %ebx
    call get_file
    cmpl $-1, %eax #fail if bad fd
    je read_done
    addl $4, %esp #pop c-args

    pushl %eax #edx, ecx already here, put eax
    call kread

read_done:
    addl $12, %esp #pop c-args
    ret
read_bad_ptr:
    addl $8, %esp
    movl $-1,%eax
    ret


write: 
    pushl %edx
    pushl %ecx
    call usr_ptr_ok
    orl %eax,%eax
    jz read_bad_ptr

    
    pushl %ebx
    call get_file
    cmpl $-1, %eax #fail if bad fd
    je read_done
    addl $4, %esp #pop c-args

    pushl %eax
    call kwrite

    addl $12, %esp #pop c-args
    ret

open: 
    pushl %ebx
    call syscall_open
    addl $4, %esp #pop c-args
    ret

close:
    pushl %ebx
    call syscall_close
    addl $4, %esp #pop c-args
    ret

getargs:
    pushl %ecx
    pushl %ebx
    call syscall_getargs
    addl $8, %esp #pop c-args
    ret
	 
vidmap:
    pushl %ebx
    call syscall_vidmap
    addl $4, %esp #pop c-args
    ret
 
set_handler:
    ret

sigreturn: 
    ret
