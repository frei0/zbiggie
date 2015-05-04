#define ASM 1
#include "tasking_funcs.h" 

.globl next_task
.align 4

/* next task 
 * inputs: none 
 * return: none 
 * function: swittches to next task in run queue
 */
next_task:
    cli
    pushal
    push $0 
    call send_eoi
    addl $4,%esp

    CALL save_queue #save current pid to current run queue entry
    CALL get_current_pcb
    movl %esp, (%eax) #save current stack to pcb

    CALL get_next_task_pid 
    pushl %eax
    CALL incr_current_active_process
    popl %eax
    cmpl  $0,%eax
    jz    launch_new_shell #if next run queue entry is empty, start a base shell
switching:
    CALL get_current_task_pid
    pushl %eax
    CALL switch_context #switch memory map
    addl $4,%esp
    CALL get_current_pcb
    movl (%eax), %esp #restore old stack w/ iret context of next task
    popal
    sti 
    iret
#executes "shell"
launch_new_shell:
    movl execstring,%ebx
    jmp kexecute

shellstring:
    .ASCIZ "shell"
