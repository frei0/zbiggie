#define ASM 1
#include "idt_linkage.h"

.globl asm_keyboard, asm_rtc, asm_pit
.align 4

# Assembly linkage for keyboard handler to 
# save all of the registers before hand
asm_keyboard:
	pushal
	call key_handler
	popal
	iret

# Assembly linkage for RTC handler to 
# save all of the registers before hand
asm_rtc:
	pushal 
	call rtc_handler
	popal
	iret

# Assembly linkage for PIT handler to 
# save all of the registers before hand
asm_pit:
	pushal
	call pit_handler
	popal
	iret
