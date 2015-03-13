#ifndef IDT_FUNCS_H
#define IDT_FUNCS_H


extern void PIC_sendEOI(unsigned char irq);
extern void common_interrupt();
extern void divide_by_zero();
extern void reserved_1();
extern void non_maskable_interrupt();
extern void breakpoint();
extern void overflow();
extern void BOUND_range_exceeded();
extern void invalid_opcode();
extern void device_not_available();
extern void double_fault();
extern void coprocessor_segment_overrun();
extern void invalid_TSS();
extern void segment_not_present();
extern void stack_segment_fault();
extern void general_protection();
extern void page_fault();
//15 Intel Reserved
extern void floating_point_error();
extern void alignment_check();
extern void machine_check();
extern void SIMD_floating_point_exception();

//20-31 Intel Reserved

//32-255
//User Defined: Call common interrupt

extern void something_went_wrong(); 

#endif