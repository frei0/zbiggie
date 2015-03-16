/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "rtc.h"
#include "x86_desc.h"
#include "lib.h"

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
