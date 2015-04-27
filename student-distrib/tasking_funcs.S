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

    CALL save_queue
    CALL get_current_pcb
    movl %esp, (%eax)

    CALL get_next_task_pid
    pushl %eax
    CALL incr_current_active_process
    popl %eax
    cmpl  $0,%eax
    jz    launch_new_shell



switching:
    CALL get_next_task_pid
    pushl %eax
    CALL switch_context #switch memory map
    addl $4,%esp
    CALL get_current_pcb
    movl (%eax), %esp #restore old stack w/ iret context of next task
    popal
    sti 
    iret

launch_new_shell:
    movl execstring,%ebx
    jmp execute

shellstring:
    .ASCIZ "shell"
