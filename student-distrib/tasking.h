#ifndef _TASKING_H_
#define _TASKING_H_
#include "zbigfs.h"
#include "types.h"
#include "terminal.h"
#define MAX_PID 7
#define MAX_FILES 8
#define NUM_THREADS 3
typedef struct pcb{
    uint32_t esp;
    int parent;
    char cmdstring[BUF_SIZE];
    FILE f[MAX_FILES];
    char present; //boolean
} pcb_t;
int current_thread;
int threads[NUM_THREADS]; //PID for each running thread 

int setup_new_process();
void switch_context(int pid);
FILE * get_file(int fd);
int get_current_pid();
int get_next_task_pid();
void save_queue();
void init_pcbs();
void free_current_pcb();

int free_fd(int fd);
int get_new_fd();

pcb_t * get_pcb(int);
pcb_t * get_current_pcb();

#endif
