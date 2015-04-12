#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
typedef struct pcb{
    uint32_t esp;
    int parent;
    int a;
    int b;
    int c;
    int d;
    int e;
    int z;
    int g;
    int h;
    int i;
    int j;
    int k;
    int l;
    FILE f[8];
    char present; //boolean
} pcb_t;

void setup_new_process();
void switch_context(int pid);
FILE * get_file(int fd);
pcb_t * get_pcb(int);
pcb_t * get_current_pcb();

#endif
