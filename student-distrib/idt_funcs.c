#include "idt_funcs.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"

extern uint8_t
INB (uint16_t port)
{
 register uint8_t ret;
 asm volatile("inb %%dx, %%al"
 : "=a" (ret)
 : "d" (port)
 );
 return ret;
}

extern uint16_t
INW (uint16_t port)
{
 register uint16_t ret;
 asm volatile("inw %%dx, %%ax"
 : "=a" (ret)
 : "d" (port)
 );
 return ret;
}

extern void common_interrupt()
{
	//clear();
	printf("common_interrupt\n");

	while(1);
}

//0
extern void divide_by_zero()
{
	//clear();
	printf("divide_by_zero\n");
	while(1);
}
//1
extern void reserved_1()
{
	printf("Intel use only\n");
	while(1);
}
//2
extern void non_maskable_interrupt()
{
	printf("non_maskable_interrupt\n");
	while(1);
}
//3
extern void breakpoint()
{
	printf("breakpoint\n");
	while(1);
}
//4
extern void overflow()
{
	printf("overflow\n");
	while(1);
}
//5
extern void BOUND_range_exceeded()
{
	printf("BOUND_range_exceeded\n");
	while(1);
}
//6
extern void invalid_opcode()
{
	printf("invalid_opcode\n");
	while(1);
}
//7
extern void device_not_available()
{
	printf("device_not_available\n");
	while(1);
}
//8
extern void double_fault()
{
	printf("double_fault\n");
	while(1);
}
//9
extern void coprocessor_segment_overrun()
{
	printf("coprocessor_segment_overrun\n");
	while(1);
}
//10
extern void invalid_TSS()
{
	printf("invalid_TSS\n");
	while(1);
}
//11
extern void segment_not_present()
{
	printf("segment_not_present\n");
	while(1);
}
//12
extern void stack_segment_fault()
{
	printf("stack_segment_fault\n");
	while(1);
}
//13
extern void general_protection()
{
	printf("general_protection\n");
	while(1);
}
//14
extern void page_fault()
{
	printf("page_fault\n");
	while(1);
}

//15 Intel Reserved

//16
extern void floating_point_error()
{
	printf("floating_point_error\n");
	while(1);
}
//17
extern void alignment_check()
{
	printf("alignment_check\n");
	while(1);
}
//18
extern void machine_check()
{
	printf("machine_check\n");
	while(1);
}
//19
extern void SIMD_floating_point_exception()
{
	printf("SIMD_floating_point_exception\n");
	while(1);
}

//dec. 20-31 Intel Reserved

//32-255
//User Defined: Call common interrupt

//0x21 - keyboard
extern void key_handler()
{
	//clear()
	char in;
	cli();
	in = INB(0x60);
	printf("%x ", in);
	sti();
	send_eoi(1);	
	//while(1);
}

extern void something_went_wrong()
{
	printf("Ughhhh we did something wrong. . . Awks \n");
	while(1);
}








