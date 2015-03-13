#ifndef IDT_FUNCS_H
#define IDT_FUNCS_H

extern void common_interrupt();
extern void divide_by_zero();
extern void reserved_1();
extern void non_maskable_interrupt();
extern void breakpoint();
extern void overflow();
extern void BOUND_range_exceeded();
extern void invalid_opcode();


#endif
