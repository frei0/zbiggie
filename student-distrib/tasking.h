#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
typedef struct pcb{
    FILE f[8];
    int parent;
    uint32_t esp;
} pcb_t;

#endif
