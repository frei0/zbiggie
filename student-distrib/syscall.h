#ifndef SYSCALL_H
#define SYSCALL_H
#include "x86_desc.h"
#include "syscall_funcs.h"
#ifndef ASM
#include "lib.h"
#include "tasking.h"

extern void syscall();
extern void halt();

#endif
#endif
