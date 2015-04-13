#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
typedef struct pcb{
    uint32_t esp;
    int parent;
    int edi;
    int esi;
    int edx;
    int ecx;
    int ebx;
    int ebp;
    int iret;
    int cs;
    int flags;
    int iesp;
    int ds;
    FILE f[8];
    char present; //boolean
} pcb_t;

void setup_new_process();
void switch_context(int pid);
FILE * get_file(int fd);
int get_current_pid();
pcb_t * get_pcb(int);
pcb_t * get_current_pcb();

#endif
