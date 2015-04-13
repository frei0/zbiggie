#ifndef SYSCALL_H
#define SYSCALL_H
#include "x86_desc.h"
#include "syscall_funcs.h"
#define SYSCALL_MAX_INDEX 9
#define EFLAG_INT_ENABLE 0x200
#ifndef ASM
#include "lib.h"
#include "tasking.h"

extern void syscall();
extern void halt();

#endif
#endif
