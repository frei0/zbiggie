#define ASM 1
#include "tasking_funcs.h" 

.globl save_current_in_queue 
.align 4

next_task:
    pushal
    CALL get_current_pcb
    movl %esp, (%eax)
    CALL save_queue
switching:
    CALL get_next_task_pid
    CALL switch_context #switch memory map
    CALL get_pcb
    movl (%eax), %esp #restore old stack w/ iret context of next task
    popal
    iret
