#ifndef _TERMINAL_H
#define _TERMINAL_H

void term_init();
void term_putc(char c);
void term_puts(char * c);
void term_clear();
void term_move_left();
void term_move_right();

#endif
