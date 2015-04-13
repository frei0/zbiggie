#include "tasking.h"
#include "terminal.h"
#include "page.h"
#include "x86_desc.h"
#include "lib.h"


#define OFFSET_8M (OFFSET_4M*2)
#define OFFSET_8K (OFFSET_4K*2)

int current_process = 0; //0 is not the shell, but the entry point

int find_free_pcb(){
    int i = 1;
    while (get_pcb(i)->present) i++;
    if (i > MAX_PID){ 
        printf("E: process limit reached\n");
        return -1;
    }
    return i;
}

pcb_t * get_pcb(int i){
    return  (pcb_t *) (OFFSET_8M - (i+1)* OFFSET_8K) ;
}

pcb_t * get_current_pcb(){
    pcb_t * my_pcb =  get_pcb(current_process);
    //printf("geting pcb: %x", my_pcb);
    return my_pcb; 
}

FILE * get_file(int fd){
    if (fd >= MAX_FILES || fd < 0) return (void *) -1;
    pcb_t * pcb_ptr = get_pcb(current_process);
    return &(pcb_ptr->f[fd]);
}

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
int free_fd(int fd){
    if (fd >= MAX_FILES || fd < 0) return -1;
    FILE * f = get_pcb(current_process)->f;
    f[fd].flags|=(~FILE_FLAG_IN_USE);
    return 0;
}

int get_current_pid()
{
    return current_process;
}

void free_current_pcb()
{
    pcb_t* pcb_ptr = get_current_pcb();
    pcb_ptr->present = 0;
}

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

void switch_context(int pid){
    current_process = pid;
    set_cr3(pid);    
    tss.esp0 = (OFFSET_8M - pid*OFFSET_8K);
}

