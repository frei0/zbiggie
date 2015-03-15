#define ASM 1
#include "idt_linkage.h"

.globl asm_keyboard, asm_rtc
.align 4

asm_keyboard:
	pushal
	call key_handler
	popal
	iret

asm_rtc:
	pushal 
	call rtc_handler
	popal
	iret