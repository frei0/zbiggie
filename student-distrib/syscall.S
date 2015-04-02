#define ASM 1
#include "syscall.h" 





syscall: 
	decl	%eax	
	cmpl 	$9, %eax
	ja		syscall_fail
	jmp 	*jump_table(,%eax,4)
	

jump_table: 	#jump table of syscalls
	.long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

syscall_fail:
	movl $-1, %eax				#FAILURE
	ret


halt:

execute: 

read: 

write: 

open: 

close:

getargs: 

vidmap:
 
set_handler:

sigreturn: 
