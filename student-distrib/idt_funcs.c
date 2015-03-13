#include "idt_funcs.h"
#include "lib.h"



extern void common_interrupt()
{
	//clear();
	printf("common_interrupt\n");
	while(1);
}

extern void divide_by_zero()
{
	//clear();
	printf("divide_by_zero\n");
	while(1);
}

extern void reserved_1()
{
	printf("Intel use only\n");
	while(1);
}

extern void non_maskable_interrupt()
{
	printf("non_maskable_interrupt\n");
	while(1);
}

extern void breakpoint()
{
	printf("breakpoint\n");
	while(1);
}

extern void overflow()
{
	printf("overflow\n");
	while(1);
}

extern void BOUND_range_exceeded()
{
	printf("BOUND_range_exceeded\n");
	while(1);
}

extern void invalid_opcode()
{
	printf("invalid_opcode\n");
	while(1);
}


