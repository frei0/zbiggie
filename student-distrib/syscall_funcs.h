#ifndef _SYSCALL_FUNCS_H
#define _SYSCALL_FUNCS_H
#define LOAD_ADDR 0x08048000
#ifndef ASM
#include "lib.h"
//Helper Functions
int exec_check(int * ptr);
void * load_exec_to_mem(const char * fname);
int buffer_parser(char * arg1, char * arg2, char * arg3, const char * s);
#endif /*ASM*/
#endif /*_SYSCALL_FUNCS_H*/
