#ifndef SYSCALL_FUNCS_H
#include "lib.h"

#define SYSCALL_FUNCS_H

int halt_call();
int execute_call(); 
int read_call(); 
int write_call(); 
int open_call(); 
int close_call();
int getargs_call(); 
int vidmap_call();
int set_handler_call(); 
int sigreturn_call(); 

//Helper Functions
int exec_check(int * ptr);
void * load_exec_to_mem();
int buffer_parser(char * arg1, char * arg2, char * arg3, char * s);

#endif
