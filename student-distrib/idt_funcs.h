#ifndef IDT_FUNCS_H
#define IDT_FUNCS_H

#define KEY_LINE		1
#define KEY_PORT		0x60
#define PIT_LINE		0
#define PRESSED_RANGE	0x59
#define DOWN_ARROW 		0x50
#define UP_ARROW		0x48
#define LEFT_ARROW		0x4B
#define RIGHT_ARROW		0x4D
#define CONTROL_DOWN	0x1D
#define LEFT_SHIFT		0x2A
#define RIGHT_SHIFT		0x36
#define CAPS_LOCK		0x3A
#define CONTROL_UP		0x9D
#define LEFT_SHIFT_UP	0xAA
#define RIGHT_SHIFT_UP	0xB6
#define BYTE_MASK		0x000000FF
#define OFFSET			32



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
extern int system_calls();

//20-31 Intel Reserved

//32-255
//User Defined: Call common interrupt
extern void key_handler();

extern void something_went_wrong();
extern volatile int rtc_f;




#endif
