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
	outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);
   
    outb(ICW2_MASTER, MASTER_8259_PORT+DATA_OFFSET);
    outb(ICW2_SLAVE, SLAVE_8259_PORT+DATA_OFFSET);
   
    outb(ICW3_MASTER, MASTER_8259_PORT+DATA_OFFSET);
    outb(ICW3_SLAVE, MASTER_8259_PORT+DATA_OFFSET);
   
    outb(ICW4, MASTER_8259_PORT+DATA_OFFSET);
    outb(ICW4, MASTER_8259_PORT+DATA_OFFSET);
}

/* Enable (unmask) the specified IRQ 
* from osdev*/
void
enable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) { // Check if master or slave
        port = MASTER_8259_PORT + DATA_OFFSET;
    } else {
        port = SLAVE_8259_PORT + DATA_OFFSET;
        irq_num -= 8; //If slave, decrement irq_num by 8
    }
    value = inb(port) | (1 << irq_num);
    outb(value, port);        
}

/* Disable (mask) the specified IRQ 
 * from osdev */
void
disable_irq(uint32_t irq_num)
{
	uint16_t port;
    uint8_t value;
 
    if(irq_num < 8) { // Check if master or slave
        port = MASTER_8259_PORT + DATA_OFFSET;
    } else {
        port = SLAVE_8259_PORT + DATA_OFFSET;
        irq_num -= 8; //If slave, decrement irq_num by 8
    }
    value = inb(port) & ~(1 << irq_num);
    outb(value, port);      
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num >= 8) // If interrupt came from slave, send EOI to both
    {
        uint32_t temp = irq_num - 8;
		outb(EOI|temp, SLAVE_8259_PORT);
    }
	outb(EOI|irq_num, MASTER_8259_PORT);
}

