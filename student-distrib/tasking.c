#include "tasking.h"
#include "terminal.h"
#include "page.h"


int current_process = 0; //0 is not the shell, but the entry point

int find_free_pcb(){
    int i = 1;
    while (get_pcb(i)->present) i++;
    return i;
}

pcb_t * get_pcb(int i){
    return  (pcb_t *) (OFFSET_4M*2 - (i+1)* OFFSET_4K * 2) ;
}

pcb_t * get_current_pcb(){return get_pcb(current_process);}

FILE * get_file(int fd){
    pcb_t * pcb_ptr = get_pcb(current_process);
    return &(pcb_ptr->f[fd]);
}


void setup_new_process(){
    int pid = find_free_pcb();
    pcb_t * pcb_ptr = get_pcb(pid);
    stdout_open(& (pcb_ptr->f[1]) );
    stdin_open(& (pcb_ptr->f[0]) );
    pcb_ptr->parent = current_process;
    init_pd(pid);
    switch_context(pid);
}

void switch_context(int pid){
    current_process = pid;
    set_cr3(pid);    
}
