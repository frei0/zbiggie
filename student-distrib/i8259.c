/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "x86_desc.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	OUTB(MASTER_8259_PORT,ICW1);
    OUTB(SLAVE_8259_PORT,ICW1);
   
    OUTB(MASTER_8259_PORT+1,ICW2_MASTER);
    OUTB(SLAVE_8259_PORT+1,ICW2_SLAVE);
   
    OUTB(MASTER_8259_PORT+1,ICW3_MASTER);
    OUTB(MASTER_8259_PORT+1,ICW3_SLAVE);
   
    OUTB(MASTER_8259_PORT+1,ICW4);
    OUTB(MASTER_8259_PORT+1,ICW4);
}

/* Enable (unmask) the specified IRQ 
* from osdev*/
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT + 1;
        irq_num -= 8;
    }
    value = INB(port) | (1 << irq_num);
    OUTB(port, value);        
}

/* Disable (mask) the specified IRQ 
 * from osdev */
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) {
        port = MASTER_8259_PORT + 1;
    } else {
        port = SLAVE_8259_PORT +1;
        irq_num -= 8;
    }
    value = INB(port) & ~(1 << irq_num);
    OUTB(port, value);      
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 8)
		OUTB(SLAVE_8259_PORT,EOI);
	OUTB(MASTER_8259_PORT,EOI);
}

