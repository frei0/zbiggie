#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
#define MAX_PID 7
typedef struct pcb{
    uint32_t esp;
    int parent;
    int flags;
    FILE f[8];
    char present; //boolean
} pcb_t;

int setup_new_process();
void switch_context(int pid);
FILE * get_file(int fd);
int get_current_pid();
void free_current_pcb();
pcb_t * get_pcb(int);
pcb_t * get_current_pcb();

#endif
