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
    //OUTB(RTC_INDEX_PORT,0x8A); //select port A, and disable NMI at same time
    //OUTB(RTC_RW_PORT,0x20);

	// enable irq 8
    OUTB(RTC_INDEX_PORT, 0x8B); //select port B, keep NMI disabled
    char current_b = INB(RTC_RW_PORT);
    OUTB(RTC_INDEX_PORT, 0x8B); //select port B, keep NMI disabled
    OUTB(RTC_RW_PORT, current_b | 0x40); //enable IR
    OUTB(RTC_INDEX_PORT, INB(RTC_INDEX_PORT)&0x7F); //enable NMI again
    enable_irq(8);
}
