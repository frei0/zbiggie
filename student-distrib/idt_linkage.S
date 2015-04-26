#define ASM 1
#include "idt_linkage.h"

.globl asm_keyboard, asm_rtc, asm_pit
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

asm_pit:
	pushal
	call pit_handler
	popal
	iret
