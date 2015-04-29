#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "page.h"
#include "tasking.h"
#define START_POS 10
#define NUM_BUFS 10
#define CHAR_W 8
#define KB_IRQ 1
#define BACKSPACE 0x08

char buffer[3][BUF_SIZE];
int cur_pos[3] = {0,0,0};
int cur_size[3] = {0,0,0};
int prev_size[3]; 
int write_x[3] = {-1,-1,-1}; 
int write_y[3] = {-1,-1,-1}; 

volatile int wait_for_nl;


int noread(){ return -1;} //cant read from stdout!
int nowrite(){ return -1;} //cant write to stdin!
TypedFileOperation ops_stdout[3] = {(TypedFileOperation) stdout_open, (TypedFileOperation) noread, (TypedFileOperation) term_write};
TypedFileOperation ops_stdin[3] = {(TypedFileOperation) stdin_open, (TypedFileOperation) term_read, (TypedFileOperation) nowrite};
int stdout_open(FILE * f){
    f->optable = ops_stdout;
    return 0;
}
int stdin_open(FILE * f){
    f->optable = ops_stdin;
    return 0;
}

/* void term_open()
 * Inputs: none
 * return: none
 * function: unmasks interrupts on keyboard's irq and
 * initializes the terminal*/
void term_open()
{
	buffer[current_terminal][0] = '\0'; 
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
    int j;
    for(j = 0; j < BUF_SIZE; j++)
	{
		buffer[current_terminal][j] = 0; 
	}
	cur_pos[current_terminal] = 0; 
	cur_size[current_terminal] = 0;
	write_x[current_terminal] = -1;
	write_y[current_terminal] = -1; 	
    puts("zbiggie term active!\n");
}

/*void term_putc(char c)
 * Inputs: a character to be displayed on the terminal
 * Returns: nothing
 * Function: prints a single character to the terminal*/
void term_putc(char c)
{
    //int i;
    cli();
    char bs_char;
    //new line, new buf
    if(c == '\n' || c == '\r') 
    {
        if(cur_size[current_terminal] < BUF_SIZE)
            buffer[current_terminal][cur_size[current_terminal]] = '\n';

        cur_pos[current_terminal] = 0;
        set_x(START_POS);
        putc_kb(c);
        //puts("zbiggie: ");
        prev_size[current_terminal] = cur_size[current_terminal];
        cur_size[current_terminal] = 0;
        write_x[current_terminal] = -1;
        write_y[current_terminal] = -1;
        wait_for_nl = 0;
    }
    //backspace
    else if(c == BACKSPACE)
    {
       bs_char = ' ';
       //if we're backspacing from the end of the line, BS with a null
       //otherwise bs with a space
       if(cur_pos[current_terminal] >= cur_size[current_terminal] -1)
       {
           bs_char = 0;
           cur_size[current_terminal] --;
       }       
       //if not the first character, delete the char then move left
       if(cur_pos[current_terminal] > 0)
       {
           if(get_screen_y(current_terminal) > write_y[current_terminal] || (get_screen_y(current_terminal) == write_y[current_terminal] 
                   && get_screen_x(current_terminal) > write_x[current_terminal]))
           {
           move_left();
           putc_kb(bs_char); 
           move_left();
           }
           cur_pos[current_terminal]--;
           buffer[current_terminal][cur_pos[current_terminal]] = bs_char;
       }
       //if first character, just delete, don't move left
       else
       {
           buffer[current_terminal][cur_pos[current_terminal]] = bs_char;
           if(get_screen_y(current_terminal) > write_y[current_terminal] ||( get_screen_y(current_terminal) == write_y[current_terminal] 
                   && get_screen_x(current_terminal) > write_x[current_terminal]))
           {
           putc(bs_char);
           move_left();
           }
       }
    }
    //if just a regular character and buffer isn't full, 
    //print it and put it in the buffer
    else if(cur_pos[current_terminal] < BUF_SIZE-1)
    {
        putc_kb(c);
        buffer[current_terminal][cur_pos[current_terminal]] = c;
        cur_pos[current_terminal] ++;
        cur_size[current_terminal]++;
    }
    sti();

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
int term_write(FILE * f, char * buf, int cnt)
{
   cli();
   int i;
   for (i = 0; i < cnt; ++i) mt_putc(buf[i]);
   switch_term_xy(current_active_process);
   write_x[current_active_process] = get_screen_x(current_active_process); 
   write_y[current_active_process] = get_screen_y(current_active_process); 
   switch_term_xy(current_terminal);
   sti();
   return cnt;
}

/* char * term_read()
 * Inputs: none
 * Returns: the last full terminal buffer terminated in a new line
 * Function: get the last terminal buffer*/
int term_read(FILE * f, char * buf, int numbytes)
{
   int i;
   wait_for_nl = 1;
   while (wait_for_nl) {}
   while(current_terminal != current_active_process){} 
   cli();
   for(i = 0; i < numbytes; i++)
   {
       //if end of line or NULL
        buf[i] = buffer[current_terminal][i];
       if(buf[i] == '\n'){
           sti();
           return i+1;  //we have a whole line
       }
   }
   sti();
   return i;
}


/* void term_clear()
 * inputs: none
 * return: none
 * function: clear the terminal*/
void term_clear()
{
    clear();
    set_pos(0, 0);
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
        cur_pos[current_terminal]--;
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
        cur_pos[current_terminal]++;
    }
}

void term_switch()
{	
        switch_term_xy(current_terminal);
		set_pos(get_screen_x(current_terminal), get_screen_y(current_terminal));
}







