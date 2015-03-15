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
	in = (char)INB(0x60);
	if(in > 59)
	{

	}
	else
	{
		putc(scan2ASCII[in]);
	}
	
	sti();
	send_eoi(1);	
	//while(1);
}

extern void something_went_wrong()
{
	printf("Ughhhh we did something wrong. . . Awks \n");
	while(1);
}


extern char scan2ASCII[256] = 
	{
		0x00, 0x00, 0x31, 0x32, 0x33, 0x34,
		0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
		0x2D, 0x3D, 0x08, 0x09, 0x71, 0x77,
		0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
		0x6F, 0x70, 0x5B, 0x5D, 0x10, 0xFF,
		0x61, 0x71, 0x64, 0x66, 0x67, 0x68,
		0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60,
		0xFF, 0x5C, 0x7A, 0x78, 0x63, 0x76,
		0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F,
		0xFF, 0x2A, 0xFF, 0x20, 0xFF
	};





