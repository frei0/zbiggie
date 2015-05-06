#ifndef _SYSCALL_FUNCS_H
#define _SYSCALL_FUNCS_H
#define LOAD_ADDR 0x08048000
#ifndef ASM
#include "lib.h"
#include "tasking.h"
//Helper Functions
int exec_check(const char * fname);
void * load_exec_to_mem(const char * fname);
void * kload_exec_to_mem(const char * fname);
void parse_input(const char * in, char * exec_buf, char * args_buf, int size);
int syscall_open(const char * name);
int syscall_close(int fd);
int syscall_getargs(char * buf, int nbytes);
int syscall_vidmap(uint8_t ** vid_ptr);
int usr_ptr_ok(const void * p, uint32_t s);
#endif /*ASM*/
#endif /*_SYSCALL_FUNCS_H*/
