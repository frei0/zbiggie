#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "zbigfs.h"

void term_open();
void term_close();
void term_init();
void term_putc(char c);
int term_puts(char * c);
void term_put_last();
void term_clear();
void term_move_left();
void term_move_right();
int term_write(FILE * f, char * str, int n);
char * term_read();
int stdout_open(FILE *f);

#endif
