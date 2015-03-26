#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#define START_POS 10
#define NUM_BUFS 10
#define BUF_SIZE 128
#define CHAR_W 8
#define KB_IRQ 1
#define BACKSPACE 0x08

char buffers[NUM_BUFS][BUF_SIZE];
static char read_ret_buf[BUF_SIZE];
int cur_buf = 0;
int cur_pos = 0; 
int cur_size = 0;

/* void term_open()
 * Inputs: none
 * return: none
 * function: unmasks interrupts on keyboard's irq and
 * initializes the terminal*/
void term_open()
{
    disable_irq(KB_IRQ);
    term_init();
}

/* void term_close()
 * Inputs: none
 * return: none
 * function: masks interrupts on keyboard's irq and
 * closes the terminal*/
void term_close()
{
    enable_irq(KB_IRQ);
    term_clear();
}

/* void term_init()
 * Inputs: none
 * return: none
 * function: initializes the terminal*/
void term_init()
{
    int i,j;
    for(i = 0; i < NUM_BUFS; i++)
    {
        for(j = 0; j < BUF_SIZE; j++)
            buffers[i][j] = 0;
    }
    puts("zbiggie: ");
    cur_pos = 0;
    cur_buf = 0;
    cur_size = 0;
}

/*void term_putc(char c)
 * Inputs: a character to be displayed on the terminal
 * Returns: nothing
 * Function: prints a single character to the terminal*/
void term_putc(char c)
{
    int i;
    char bs_char;
    //new line, new buf
    if(c == '\n' || c == '\r') 
    {
        if(cur_size < BUF_SIZE)
            buffers[cur_buf][cur_size] = '\n';

        cur_buf ++;
        cur_buf %=NUM_BUFS;
        cur_pos = 0;
        set_x(START_POS);
        for(i = 0; i < BUF_SIZE; i++) 
            buffers[cur_buf][i] = 0;
        putc_kb(c);
        puts("zbiggie: ");
        cur_size = 0;
    }
    //backspace
    else if(c == BACKSPACE)
    {
       bs_char = ' ';
       //if we're backspacing from the end of the line, BS with a null
       //otherwise bs with a space
       if(cur_pos >= cur_size -1)
       {
           bs_char = 0;
           cur_size --;
       }       
       //if not the first character, delete the char then move left
       if(cur_pos > 0)
       {
           move_left();
           putc_kb(bs_char); 
           move_left();
           cur_pos--;
           buffers[cur_buf][cur_pos] = bs_char;
       }
       //if first character, just delete, don't move left
       else
       {
           buffers[cur_buf][cur_pos] = bs_char;
           putc(bs_char);
           move_left();
       }
    }
    //if just a regular character and buffer isn't full, 
    //print it and put it in the buffer
    else if(cur_pos < BUF_SIZE-1)
    {
        putc_kb(c);
        buffers[cur_buf][cur_pos] = c;
        cur_pos ++;
        cur_size++;
    }

}

/* int term_puts(char * c)
 * Inputs: a null terminated string to be displayed on the terminal
 * Returns: number of bytes successfully written  
 * Function: prints a string to the terminal*/
int term_puts(char * str)
{
   int i;
   if(str == NULL)
       return 0;
   for(i = 0; i < BUF_SIZE; i++)
   {
       if(str[i] == NULL)
           break;
       term_putc(str[i]);
   }
   return i; 
}

/* int term_write(char * c)
 * Inputs: a null terminated string to be displayed on the terminal
 * Returns: number of bytes successfully written  
 * Function: prints a string to the terminal*/
int term_write(char * str)
{
    return term_puts(str);
}

/* char * term_read()
 * Inputs: none
 * Returns: the last full terminal buffer terminated in a new line
 * Function: get the last terminal buffer*/
char * term_read()
{
   int i;
   int prev_buf;

   prev_buf = cur_buf - 1;
   if(prev_buf < 0)
       prev_buf = NUM_BUFS -1;
   for(i = 0; i < BUF_SIZE; i++)
   {
       //if end of line or NULL
       if(buffers[prev_buf][i] == '\n' || buffers[prev_buf][i] == NULL)
           break;
       read_ret_buf[i] = buffers[prev_buf][i];
   }
   if(read_ret_buf[i] != '\n')
   {
       read_ret_buf[i] = '\n';
   }
   return read_ret_buf;
}

//dumb history, just puts last buffer in. real history needs a
//history file
void term_put_last()
{
    /*TODO: history file*/
   int i;
   int prev_buf;

   prev_buf = cur_buf - 1;
   if(prev_buf < 0)
       prev_buf = NUM_BUFS -1;
   for(i = 0; i < BUF_SIZE; i++)
   {
       //if end of line or NULL
       if(buffers[prev_buf][i] == '\n' || buffers[prev_buf][i] == NULL)
           break;
       term_putc(buffers[prev_buf][i]);
   }

}

/* void term_clear()
 * inputs: none
 * return: none
 * function: clear the terminal*/
void term_clear()
{
    clear();
    set_pos(0, 0);
    //term_init();
}

/* void term_move_left()
 * inputs: none
 * return: none
 * function: move the cursor left*/
void term_move_left()
{
    if(cur_pos > 0)
    {
        move_left();
        cur_pos--;
    }
}

/* void term_move_right()
 * inputs: none
 * return: none
 * function: move the cursor right*/
void term_move_right()
{
    if(cur_pos < cur_size)
    {
        move_right();
        cur_pos++;
    }
}

