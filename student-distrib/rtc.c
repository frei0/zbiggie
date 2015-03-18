/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"
#include "idt_funcs.h"

/* Initialize the rtc
 *
 * interrupts must be disabled
 */
void
rtc_init(void)
{
	// enable irq 8
    outb(NO_NMI_B, RTC_INDEX_PORT); //select port B, keep NMI disabled
    char current_b = inb(RTC_RW_PORT);
    outb(NO_NMI_B, RTC_INDEX_PORT); //select port B, keep NMI disabled
    outb(current_b | RTC_ENABLE, RTC_RW_PORT); //enable IR
    outb( inb(RTC_INDEX_PORT) & NMI_ON, RTC_INDEX_PORT); //enable NMI again
    enable_irq(RTC_LINE);
}

int change_rtc_freq(int rate)
{
	if(rate<3 || rate >15)
	{
		return -1;
	}
	cli();
	outb(NO_NMI_A, RTC_INDEX_PORT); //select port A, keep NMI disabled
	char previous_rate = inb(RTC_RW_PORT);
	outb(NO_NMI_A, RTC_INDEX_PORT); // reselect A
	outb((previous_rate & RATE_MASK)|rate, RTC_RW_PORT);
	outb( inb(RTC_INDEX_PORT) & NMI_ON, RTC_INDEX_PORT); //enable NMI again
	sti();
	return 0;

}

int rtc_read(void)
{
	rtc_f = 1;
	while(rtc_f);
	return 0;
}

int rtc_open(unsigned int frequency)
{
	int new_rate;
	switch(frequency)
	{
		case 1:
			new_rate = 15;
			break;
		case 2:
			new_rate = 14;
			break;
		case 4:
			new_rate = 13;
			break;
		case 8:
			new_rate = 12;
			break;
		case 16:
			new_rate = 11;
			break;
		case 32:
			new_rate = 10;
			break;
		case 64:
			new_rate = 9;
			break;
		case 128:
			new_rate = 8;
			break;
		case 256:
			new_rate = 7;
			break;
		case 512:
			new_rate = 6;
			break;
		case 1024:
			new_rate = 5;
			break;
		default:
			return -1;
	}
	return change_rtc_freq(new_rate);
}
