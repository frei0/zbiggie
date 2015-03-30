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
	//Check to see if it is outside the range of the RTC
	if(rate<= MAX_RATE || rate > MIN_RATE)
	{
		return -1;
	}
	//Change RTC frequency
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
	//Set the rtc flag to one then wait for it to be reset
	rtc_f = 1;
	while(rtc_f);
	return 0;
}

int rtc_write(FILE *f, unsigned int * frequency, int numbytes)
{
	if (numbytes!=4) return -1;
	int new_rate;
	//Switch statement to check for valid frequency
	switch(*frequency)
	{
		case HZ_2:
			new_rate = rate_HZ_2;
			break;
		case HZ_4:
			new_rate = rate_HZ_4;
			break;
		case HZ_8:
			new_rate = rate_HZ_8;
			break;
		case HZ_16:
			new_rate = rate_HZ_16;
			break;
		case HZ_32:
			new_rate = rate_HZ_32;
			break;
		case HZ_64:
			new_rate = rate_HZ_64;
			break;
		case HZ_128:
			new_rate = rate_HZ_128;
			break;
		case HZ_256:
			new_rate = rate_HZ_256;
			break;
		case HZ_512:
			new_rate = rate_HZ_512;
			break;
		case HZ_1024:
			new_rate = rate_HZ_1024;
			break;
		default:
			return -1;
	}
	//Change the rtc with the corresponding rate
	return change_rtc_freq(new_rate);
}

int rtc_open()
{
	//Resets the rtc to the slowest rate
	int default_rate = MIN_RATE;
	return change_rtc_freq(default_rate);
}

int rtc_close(void)
{
	return 0;
}
