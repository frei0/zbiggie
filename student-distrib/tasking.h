#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
typedef struct pcb{
    uint32_t esp;
    uint32_t ebp;
    int parent;
    FILE f[8];
    char present; //boolean
} pcb_t;

void setup_new_process();
void switch_context(int pid);
FILE * get_file(int fd);
pcb_t * get_pcb(int);
pcb_t * get_current_pcb();

#endif
