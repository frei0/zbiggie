#include "idt_funcs.h"
#include "terminal.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"
#include "rtc.h"
#include "page.h"
#define USER_EXCEPT_CODE 256
#define LOWER_PRINT_BOUND 1
#define UPPER_PRINT_BOUND 123
#define UPPER_LETTER_BOUND 123
#define LOWER_LETTER_BOUND 96

/*
* void ece391_halt();
*   Inputs: code, integer for which terminal to halt in
*   Return: none
*   Function: Halts a process in the active terminal
*/

void ece391_halt(int code){
    asm volatile("movl $1, %%eax; movl %0, %%ebx; int $0x80" : : "r"(code) : "eax", "ebx");
}
char scan2ASCII[256] = 
	{
		0x00, 0x00, 0x31, 0x32, 0x33, 0x34,
		0x35, 0x36, 0x37, 0x38, 0x39, 0x30,
		0x2D, 0x3D, 0x08, 0x09, 0x71, 0x77,
		0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
		0x6F, 0x70, 0x5B, 0x5D, 0x0A, 0xFF,
		0x61, 0x73, 0x64, 0x66, 0x67, 0x68,
		0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0x60,
		0xFF, 0x5C, 0x7A, 0x78, 0x63, 0x76,
		0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F,
		0xFF, 0x2A, 0xFF, 0x20, 0xFF,
	};

char shift2ASCII[256] = 
	{
		0x00, 0x00, 0x21, 0x40, 0x23, 0x24,
		0x25, 0x5E, 0x26, 0x2A, 0x28, 0x29,
		0x5F, 0x2B, 0x08, 0x09, 0x71, 0x77,
		0x65, 0x72, 0x74, 0x79, 0x75, 0x69,
		0x6F, 0x70, 0x7B, 0x7D, 0x0A, 0xFF,
		0x61, 0x73, 0x64, 0x66, 0x67, 0x68,
		0x6A, 0x6B, 0x6C, 0x3A, 0x22, 0x7E,
		0xFF, 0x7C, 0x7A, 0x78, 0x63, 0x76,
		0x62, 0x6E, 0x6D, 0x3C, 0x3E, 0x3F,
		0xFF, 0x2A, 0xFF, 0x20, 0xFF,
	};
char notPrintableArray[128] = 
    {
        0x0F, 0x37, 0x3E, 0x3F, 0x40, 0x41, 
		0x42, 0x43, 0x44, 0x46, 0x47, 0x49, 
		0x4A, 0x4E, 0x4F, 0x52, 0x51, 0x71, 
		0x52, 0x53, 0x57, 0x58, 
    };

int shift_l_flag[3] = {0}, shift_r_flag[3] = {0}, ctrl_flag[3] = {0};
int caps_lock_flag = 0, alt_flag = 0;

volatile int rtc_f = 0;

/*
* uint8_t INB(uint8_t port);
*   Inputs: port, what port to read a byte from
*   Return: what was read
*   Function: Read a byte froma a port
*/
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

/*
* uint16_t INW(uint16_t port);
*   Inputs: port, what port to read a word from
*   Return: what was read
*   Function: Read a word froma a port
*/
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

/*
* void common_interrupt();
*   Inputs: none
*   Return: none
*   Function: Handler for an unkown interrupt
*/
extern void common_interrupt()
{
	//clear();
	printf("common_interrupt\n");

	while(1);
}

/*
* void divide_by_zero();
*   Inputs: none
*   Return: none
*   Function: Handler for a divide by zero exception
*/
extern void divide_by_zero()
{
	//clear();
	printf("divide_by_zero\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void reserved_1();
*   Inputs: none
*   Return: none
*   Function: Interrupt space reserved for intel
*/
extern void reserved_1()
{
	printf("Intel use only\n");
	while(1);
}
/*
* void non_maskable_interrupt();
*   Inputs: none
*   Return: none
*   Function: Handler for a non-maskable interrupt exception
*/
extern void non_maskable_interrupt()
{
	printf("non_maskable_interrupt\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void breakpoint();
*   Inputs: none
*   Return: none
*   Function: Handler for a breakpoint exception
*/
extern void breakpoint()
{
	printf("breakpoint\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void overflow();
*   Inputs: none
*   Return: none
*   Function: Handler for an overflow exception
*/
extern void overflow()
{
	printf("overflow\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void BOUND_range_exceeded();
*   Inputs: none
*   Return: none
*   Function: Handler for an out of bounds exception
*/
extern void BOUND_range_exceeded()
{
	printf("BOUND_range_exceeded\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void invalid_opcode();
*   Inputs: none
*   Return: none
*   Function: Handler for an invalid opcode exception
*/
extern void invalid_opcode()
{
	printf("invalid_opcode\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void device_not_available();
*   Inputs: none
*   Return: none
*   Function: Handler for a device not available exception
*/
extern void device_not_available()
{
	printf("device_not_available\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void double_fault();
*   Inputs: none
*   Return: none
*   Function: Handler for a double fault exception
*/
extern void double_fault()
{
	printf("double_fault\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void coprocessor_segment_overrun();
*   Inputs: none
*   Return: none
*   Function: Handler for a coprocessor segment overrun exception
*/
extern void coprocessor_segment_overrun()
{
	printf("coprocessor_segment_overrun\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void invalid_TSS();
*   Inputs: none
*   Return: none
*   Function: Handler for an invalid task state segment exception
*/
extern void invalid_TSS()
{
	printf("invalid_TSS\n");
	while(1);
}
/*
* void segment_not_present();
*   Inputs: none
*   Return: none
*   Function: Handler for a segment not present exception
*/
extern void segment_not_present()
{
	printf("segment_not_present\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void stack_segment_fault();
*   Inputs: none
*   Return: none
*   Function: Handler for a stack segment fault exception
*/
extern void stack_segment_fault()
{
	printf("stack_segment_fault\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void general_protection();
*   Inputs: none
*   Return: none
*   Function: Handler for a general protection exception
*/
extern void general_protection()
{
    int x;
    //Inline assembly grabs error code
    asm volatile("popl %%ecx\n \
             movl %%ecx, %0"
             :"=r" (x)
             :
             :"%ecx"
            );
	printf("general_protection: %x\n",x);
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void page_fault();
*   Inputs: none
*   Return: none
*   Function: Handler for a page fault exception
*/
extern void page_fault()
{
    int x; 
    //Inline assembly grabs error code
    asm volatile("popl %%ecx\n \
             movl %%ecx, %0"
             :"=r" (x)
             :
             :"%ecx"
            );
	printf("page_fault: %x\n", x);
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}

//15 Intel Reserved

/*
* void floating_point_error();
*   Inputs: none
*   Return: none
*   Function: Handler for a floating point error exception
*/
extern void floating_point_error()
{
	printf("floating_point_error\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void alignment_check();
*   Inputs: none
*   Return: none
*   Function: Handler for an alignment check exception
*/
extern void alignment_check()
{
	printf("alignment_check\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void machine_check();
*   Inputs: none
*   Return: none
*   Function: Handler for a machine check exception
*/
extern void machine_check()
{
	printf("machine_check\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
/*
* void SIMD_floating_point_exception();
*   Inputs: none
*   Return: none
*   Function: Handler for a SIMD floating point exception
*/
extern void SIMD_floating_point_exception()
{
	printf("SIMD_floating_point_exception\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}

//dec. 20-31 Intel Reserved

//32-255
//User Defined: Call common interrupt

//0x21 - keyboard
/*
* void key_handler();
*   Inputs: none
*   Return: none
*   Function: Handler for a keyboard interrupt. Certain
*			  key presses either print or call functions
*/
extern void key_handler()
{
	
	char in;
    int i;
    char notPrintable = 0;
	cli();
	//Read the input
	in = (char)inb(KEY_PORT);


	int isALetter = (scan2ASCII[(int)in] > LOWER_LETTER_BOUND) && (scan2ASCII[(int)in] < UPPER_LETTER_BOUND);
	int isPrintable = (((int)in > LOWER_PRINT_BOUND) && ((int)in < UPPER_PRINT_BOUND));

	//printf("%x", in); 
	//Check if input is non-printable
    for(i = 0; i < 128; i ++)
    {
       if(in == notPrintableArray[i])
           notPrintable = 1;
    }
    if(notPrintable)
    {
		//do nothing!	
    }
    //Check all of the cases for the character scan code
    else if (in == DOWN_ARROW)
	{
		//do nothing
        //term_write(0,"hi!",3);
        down_hist();
	}
    else if(in == UP_ARROW)
    {
        up_hist();
    }
    else if (in == LEFT_ARROW)
	{
		term_move_left(); 
	}
    else if (in == RIGHT_ARROW)
	{
		term_move_right(); 
	}
	else if ( in == ALT_PRESS )
	{
		alt_flag = 1; 
		//term_write(0, "ON!", 3);
	}
    else if (in == CONTROL_DOWN)
	{
		ctrl_flag[current_terminal] = 1; 
	}
    else if (in == LEFT_SHIFT)
	{
		shift_l_flag[current_terminal] = 1; 
	}
    else if (in == RIGHT_SHIFT)
	{
		shift_r_flag[current_terminal] = 1;
	}
    else if (in == CAPS_LOCK)
	{
		caps_lock_flag = !caps_lock_flag; 
	}
    else if ((BYTE_MASK & in) == CONTROL_UP)
	{
		ctrl_flag[current_terminal] = 0; 
	}
    else if ((BYTE_MASK & in) == LEFT_SHIFT_UP)
	{
		shift_l_flag[current_terminal] = 0; 
	}
    else if ((BYTE_MASK & in) == RIGHT_SHIFT_UP)
	{
		shift_r_flag[current_terminal] = 0; 
	}
	else if  ((BYTE_MASK & in) == ALT_UP)
	{
		alt_flag = 0; 
		//term_write(0,"OFF",3); 
	}
    else if(isPrintable)
	{
        if ( (in == F_1) && alt_flag )
		{
			switch_video(0);
			term_switch(); 
			//printf("FUCKKKKK TERM 1 HERE WE GO SON");
		}
		else if ( (in == F_2) && alt_flag )
		{
			switch_video(1);
			term_switch(); 
			//printf("TERRRRMINAL 2 DAWGS IT GET HOT HERE");	
		}
		else if ( (in == F_3) && alt_flag )
		{
			switch_video(2);
			term_switch(); 
			//printf("FUCK EYAAAAA. BIGGIE NUMBA 3");
		}
		else if ( (scan2ASCII[(int)in] == 'l') && ctrl_flag[current_terminal])
        {
            term_clear();
            term_init();
            
        }
        /*else if ( (scan2ASCII[(int)in] == 'c') && ctrl_flag[current_terminal])
        {
            ece391_halt(0);
            
        }*/
        else if(isALetter && ((shift_r_flag[current_terminal] || shift_l_flag[current_terminal]) ^ caps_lock_flag))
		{
			term_putc(scan2ASCII[(int)in] - OFFSET);
		}
		else if(shift_r_flag[current_terminal] || shift_l_flag[current_terminal])
		{
			term_putc(shift2ASCII[(int)in]); 
		}
		else 
		{
            term_putc(scan2ASCII[(int)in]); 
		}
    }

	sti();
	send_eoi(KEY_LINE);	
}
/*
* void rtc_handler();
*   Inputs: none
*   Return: none
*   Function: Handler for an rtc interrupt
*/
extern void rtc_handler()
{
	cli();
	outb(NO_NMI_C, RTC_INDEX_PORT); //Turn of NMI and select C port
	inb(RTC_RW_PORT ); //Read data and throw it out to clear buffer
	rtc_f = 0; 
    outb( inb(RTC_INDEX_PORT) & NMI_ON, RTC_INDEX_PORT); //enable NMI again
    //test_interrupts();
	sti();
	send_eoi(RTC_LINE);
}

/*
* void pit_handler();
*   Inputs: none
*   Return: none
*   Function: Handler for a PIT interrupt
*/

extern void pit_handler()
{
	cli();
	//printf("got pit");
	sti();
	send_eoi(PIT_LINE);
}
/*
* void something_went_wrong();
*   Inputs: none
*   Return: none
*   Function: If something went extremely outside of out expectations
*			  this is called
*/

extern void something_went_wrong()
{
	printf("Ughhhh we did something wrong. . . Awks \n");
	while(1);
}






