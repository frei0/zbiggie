#ifndef SYSCALL_FUNCS_H
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

#endif
