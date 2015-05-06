#ifndef _TERMINAL_H
#define _TERMINAL_H
#define BUF_SIZE 128
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
int term_write(FILE * f, char * buf, int n);
int term_read(FILE * f, char * buf, int n);
int stdout_open(FILE *f);
int stdin_open(FILE *f);
void term_switch();
void save_hist();
void up_hist();
void down_hist();

extern int write_x[3]; 
extern int write_y[3]; 
#endif
