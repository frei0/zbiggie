#include "idt_funcs.h"
#include "terminal.h"
#include "lib.h"
#include "x86_desc.h"
#include "i8259.h"
#include "rtc.h"
#include "ece391syscall.h"
#define USER_EXCEPT_CODE 256

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
        0x0F, 0x37, 0x38, 0x3B, 0x3C, 0x3D, 
        0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43,
        0x44, 0x46, 0x47, 0x49, 0x4A, 0x4E, 
        0x4F,  0x52, 0x51, 0x71, 0x52, 0x53, 
        0x57, 0x58, 
    };

int shift_l_flag = 0, shift_r_flag = 0, caps_lock_flag = 0, ctrl_flag = 0;

volatile int rtc_f = 0;

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
    ece391_halt(USER_EXCEPT_CODE);
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
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//3
extern void breakpoint()
{
	printf("breakpoint\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//4
extern void overflow()
{
	printf("overflow\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//5
extern void BOUND_range_exceeded()
{
	printf("BOUND_range_exceeded\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//6
extern void invalid_opcode()
{
	printf("invalid_opcode\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//7
extern void device_not_available()
{
	printf("device_not_available\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//8
extern void double_fault()
{
	printf("double_fault\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//9
extern void coprocessor_segment_overrun()
{
	printf("coprocessor_segment_overrun\n");
    ece391_halt(USER_EXCEPT_CODE);
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
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//12
extern void stack_segment_fault()
{
	printf("stack_segment_fault\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//13
extern void general_protection()
{
    int x; 
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
//14
extern void page_fault()
{
	printf("page_fault\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}

//15 Intel Reserved

//16
extern void floating_point_error()
{
	printf("floating_point_error\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//17
extern void alignment_check()
{
	printf("alignment_check\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//18
extern void machine_check()
{
	printf("machine_check\n");
    ece391_halt(USER_EXCEPT_CODE);
	while(1);
}
//19
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
extern void key_handler()
{
	
	char in;
    int i;
    char notPrintable = 0;
	cli();
	in = (char)inb(KEY_PORT);

	int isALetter = (scan2ASCII[(int)in] > 96) && (scan2ASCII[(int)in] < 123);
	int isPrintable = (((int)in > 1) && ((int)in < 123));

    for(i = 0; i < 128; i ++)
    {
       if(in == notPrintableArray[i])
           notPrintable = 1;
    }
    if(notPrintable)
    {
    }
    else if (in == DOWN_ARROW)
	{
		//do nothing
        
        term_write(0,"hi!",3);
	//	term_puts(term_read()); 
	}
    else if(in == UP_ARROW)
    {
        //do nothing!;
    }
    else if (in == LEFT_ARROW)
	{
		term_move_left(); 
	}
    else if (in == RIGHT_ARROW)
	{
		term_move_right(); 
	}
    else if (in == CONTROL_DOWN)
	{
		ctrl_flag = 1; 
	}
    else if (in == LEFT_SHIFT)
	{
		shift_l_flag = 1; 
	}
    else if (in == RIGHT_SHIFT)
	{
		shift_r_flag = 1;
	}
    else if (in == CAPS_LOCK)
	{
		caps_lock_flag = !caps_lock_flag; 
	}
    else if ((BYTE_MASK & in) == CONTROL_UP)
	{
		ctrl_flag = 0; 
	}
    else if ((BYTE_MASK & in) == LEFT_SHIFT_UP)
	{
		shift_l_flag = 0; 
	}
    else if ((BYTE_MASK & in) == RIGHT_SHIFT_UP)
	{
		shift_r_flag = 0; 
	}
    else if(isPrintable)
	{
        if ( (scan2ASCII[(int)in] == 'l') && ctrl_flag)
        {
            term_clear();
            term_init();
            
        }
        else if(isALetter && ((shift_r_flag || shift_l_flag) ^ caps_lock_flag))
		{
			term_putc(scan2ASCII[(int)in] - OFFSET);
		}
		else if(shift_r_flag || shift_l_flag)
		{
			term_putc(shift2ASCII[(int)in]); 
		}
		else 
		{
        //else if(scan2ASCII[(int)in] == 'j')
         //   term_puts("you hit j!");
            term_putc(scan2ASCII[(int)in]); 
		}
    }

	sti();
	send_eoi(KEY_LINE);	
}

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

extern void something_went_wrong()
{
	printf("Ughhhh we did something wrong. . . Awks \n");
	while(1);
}

extern int system_calls()
{
	printf("system_calls to be set up later\n");
	return 0;
}






