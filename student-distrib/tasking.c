#include "tasking.h"
#include "terminal.h"
#include "page.h"
#include "x86_desc.h"
#include "lib.h"
#include "syscall_funcs.h"

#define OFFSET_8M (OFFSET_4M*2)
#define OFFSET_8K (OFFSET_4K*2)

int processes[NUM_PROCESSES] = {0}; //PID for each running thread 
int current_process = 0; //0 is not the shell, but the entry point
int current_active_process = 0; //has value 0,1,2

/*
*   int find_free_pcb
*   Inputs: none 
*   Return Value: pcb num 
*	Function: finds next free pcb
*/
int find_free_pcb(){
    int i = 1;
    while (get_pcb(i)->present) i++;
    if (i > MAX_PID){ 
        printf("E: process limit reached\n");
        return -1;
    }
    return i;
}

/*
*   pcb_t * get_pcb(int i) 
*   Inputs: none
*   Return Value: pointer to pcb
*	Function: gets ptr to pcb i  
*/
pcb_t * get_pcb(int i){
    return  (pcb_t *) (OFFSET_8M - (i+1)* OFFSET_8K) ;
}

/*
*   void init_pcbs() 
*   Inputs: none
*   Return Value: none  
*	Function: initializes all pcbs to 0 
*/
void init_pcbs(){
    int i;
    for (i = 0; i <= MAX_PID; ++i)
        get_pcb(i)->present = 0;
}

/*
*   pcb_t * get_current_pcb()
*   Inputs: none
*   Return Value: pcb_t ptr
*	Function: get ptr to current pcb 
*/
pcb_t * get_current_pcb(){
    pcb_t * my_pcb =  get_pcb(current_process);
    return my_pcb; 
}

/*
*   FILE * get_file(int fd) 
*   Inputs: int for fd 
*   Return Value: file ptr
*	Function: gets file array for fd 
*/
FILE * get_file(int fd){
    if (fd >= MAX_FILES || fd < 0) return (void *) -1;
    FILE * f = get_pcb(current_process)->f;
    if (0 == (f[fd].flags & FILE_FLAG_IN_USE)) return (void *) -1;
    return &(f[fd]);
}

/*
*   int get_new_fd() 
*   Inputs: none
*   Return Value: int fd
*	Function: gets new fd
*/
int get_new_fd(){
    FILE * f = get_pcb(current_process)->f;
    int i = 0;
    for (;i<MAX_FILES;++i){
        if (0 == (f[i].flags & FILE_FLAG_IN_USE)){
            f[i].flags|=FILE_FLAG_IN_USE;
            return i;
        }
    }
    return -1;
}
/*
*   int free_fd(int fd) 
*   Inputs: int fd 
*   Return Value: returns fail or success val
*	Function: frees fd 
*/
int free_fd(int fd){
    if (fd >= MAX_FILES || fd < 2) return -1;
    FILE * f = get_pcb(current_process)->f;
    if (0 == (f[fd].flags & FILE_FLAG_IN_USE)) return -1;
    f[fd].flags&=(~FILE_FLAG_IN_USE);
    return 0;
}

/*
*   int get_current_pid()
*   Inputs: none 
*   Return Value: returns current pid
*	Function: returns current pid 
*/
int get_current_pid()
{
    return current_process;
}

/*
*   int get_next_task_pid() 
*   Inputs: none
*   Return Value: next pid
*	Function: returns pid for next task in queue
*/
int get_next_task_pid()
{
    return processes[(current_active_process + 1) % NUM_PROCESSES];
}

/*
*   int get_current_task_pid() 
*   Inputs: none
*   Return Value: int current pid
*	Function: returns current pid in queue
*/
int get_current_task_pid()
{
    return processes[current_active_process];
}

/*
*   void free_current_pcb()
*   Inputs: none
*   Return Value: none
*	Function: frees current pcb
*/
void free_current_pcb()
{
    pcb_t* pcb_ptr = get_current_pcb();
    int i;
    for (i = 0; i < MAX_FILES; ++i){
        syscall_close(i);
    }
    pcb_ptr->present = 0;
}

/*
*   void save_queue() 
*   Inputs: none 
*   Return Value: none
*	Function: saves current pid to queue
*/
void save_queue()
{
    processes[current_active_process] = current_process;
}
/*
*   int setup_new_process() 
*   Inputs: none
*   Return Value: -1 fail or 0 success
*	Function: allocates resources for new process and switches to it
*/
int setup_new_process(){
    int pid = find_free_pcb();
    if (pid==-1) return -1;
    pcb_t * pcb_ptr = get_pcb(pid);
    pcb_t newpcb = {.present = 1, .parent = current_process, .f = {{0}}};
    *pcb_ptr = newpcb;

    init_pd(pid);
    switch_context(pid);
    stdin_open(& (pcb_ptr->f[get_new_fd()]) );
    stdout_open(& (pcb_ptr->f[get_new_fd()]) );
    return 0;
}
/*
*   void switch_context(int pid) 
*   Inputs: int pid
*   Return Value: none
*	Function: switches memory map
*/

void switch_context(int pid){
//    printf("switching to context of pid %d\n", pid);
    cli();
    current_process = pid;
    set_vmem_table(current_active_process);
	set_cr3(pid);    
    tss.esp0 = (OFFSET_8M - pid*OFFSET_8K);
    sti();
}

/*
*   void incr_current_active_process() 
*   Inputs: none
*   Return Value: none
*	Function: increments current process 
*/
void incr_current_active_process(){
    current_active_process = (current_active_process + 1) % NUM_PROCESSES;
}

char * execstring = "shell";

