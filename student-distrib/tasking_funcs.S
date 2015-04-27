#define ASM 1
#include "tasking_funcs.h" 

.globl next_task
.align 4

next_task:
    cli
    pushal
    push $0 
    call send_eoi
    addl $4,%esp

    CALL get_next_task_pid
    pushl %eax
    CALL incr_current_active_process
    popl %eax
    cmpl  $0,%eax
    jz    launch_new_shell

    CALL get_current_pcb
    movl %esp, (%eax)
    CALL save_queue


switching:
    CALL get_next_task_pid
    CALL switch_context #switch memory map
    CALL get_pcb
    movl (%eax), %esp #restore old stack w/ iret context of next task
    popal
    sti 
    iret

launch_new_shell:
    CALL launch_shell

shellstring:
    .ASCIZ "shell"
