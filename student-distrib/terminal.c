#include "lib.h"

#define START_POS 10
#define NUM_BUFS 10
#define BUF_SIZE 128
#define CHAR_W 8

int buffers[NUM_BUFS][BUF_SIZE];
int cur_buf = 0;
int cur_pos = 0; 
int cur_size = 0;

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


void term_putc(char c)
{
    int i;
    char bs_char;
    //new line, new buf
    if(c == '\n' || c == '\r') 
    {
        if(cur_pos < BUF_SIZE)
            //buffers[cur_buf][cur_pos] = c;

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
    else if(c == 0x08)
    {
       bs_char = ' ';
       if(cur_pos >= cur_size -1)
       {
           bs_char = 0;
           cur_size --;
       }       
       if(cur_pos > 0)
       {
           move_left();
           putc_kb(bs_char); 
           move_left();
           cur_pos--;
           buffers[cur_buf][cur_pos] = bs_char;
       }
       else
       {
           buffers[cur_buf][cur_pos] = bs_char;
           putc(bs_char);
           move_left();
       }
    }

    else if(cur_pos < BUF_SIZE)
    {
        putc_kb(c);
        buffers[cur_buf][cur_pos] = c;
        cur_pos ++;
        cur_size++;
    }

}

void term_puts(char * str)
{
   int i;
   if(str == NULL)
       return;
   for(i = 0; i < BUF_SIZE; i++)
   {
       if(str[i] == NULL)
           break;
       term_putc(str[i]);
   }
   
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
       if(buffers[prev_buf][i] == NULL)
           break;
       term_putc(buffers[prev_buf][i]);
   }

}

void term_clear()
{
    clear();
    set_pos(0, 0);
    term_init();
}

void term_move_left()
{
    if(cur_pos > 0)
    {
        move_left();
        cur_pos--;
    }
}

void term_move_right()
{
    if(cur_pos < cur_size)
    {
        move_right();
        cur_pos++;
    }
}

