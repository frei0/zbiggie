#ifndef SYSCALL_H
#define SYSCALL_H
#include "x86_desc.h"
#include "syscall_funcs.h"
#ifndef ASM
#include "lib.h"
#include "tasking.h"

#define MAX_SYS_INDEX 9
#define EFLAG_INT_ENABLE 0x200
#define ONE_BYTE		4
#define THREE_BYTE	

extern void syscall();
extern void halt();

#endif
#endif
