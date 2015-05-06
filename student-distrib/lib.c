/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab
 */

#include "lib.h"
#include "page.h"
#define VIDEO 0xB8000
#define BS 0x08
#define NUM_COLS 80
#define NUM_ROWS 25
#define SCREEN_W 320
#define CHAR_W 8
#define MASK 0xFF
#define C_LOC1 0x0E
#define C_LOC2 0x0F
#define CURSOR_BIT 0x3D4
#define CURSOR_OUT 0x3D5
static int screen_x[NUM_PROCESSES]; //the current terminal's x and y, not bg term
static int screen_y[NUM_PROCESSES];
int term_xs[NUM_PROCESSES] = {0};
int term_ys[NUM_PROCESSES] = {0};
int term_bigys[NUM_PROCESSES] = {0};
int prev_term = 0;
static int biggest_y[NUM_PROCESSES];
static char* map_video_mem = (char *)VIDEO;
char * video_mem = (char *) ((NUM_PDS+1)*OFFSET_4M + 3*OFFSET_4K);
char attribs[NUM_PROCESSES] = {0x30, 0x40, 0x50};

/*
* void clear(void);
*   Inputs: void
*   Return Value: none
*	Function: Clears video memory
*/

void
clear(void)
{
    int32_t i;
	cli(); 
    for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = attribs[current_terminal];
    }
	screen_x[current_terminal] = screen_y[current_terminal] = biggest_y[current_terminal] = 0;
	sti();
}

/*
* 	int get_screen_x(int term));
*   Inputs: terminal num 
*   Return Value: screen_x 
*	Function: returns screen_x for the desired terminal 
*/
int get_screen_x(int term)
{
	return screen_x[term];
}

/*
* 	int get_screen_y(int term));
*   Inputs: terminal num 
*   Return Value: screen_y 
*	Function: returns screen_y for the desired terminal 
*/
int get_screen_y(int term)
{
	return screen_y[term];
}

/*
* 	void set_pos(int x, int y) 
*   Inputs: x, y 
*   Return Value: none 
*	Function: sets screen_x and screen_y as well as the cursor position
*/
void
set_pos(int x, int y)
{
	cli();
    screen_x[current_terminal] = x;
    screen_y[current_terminal] = y;
    cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 
	sti();
}
/*
* 	void set_x(int x) 
*   Inputs: x 
*   Return Value: none 
*	Function: sets screen_x 
*/
void
set_x(int x)
{
    screen_x[current_terminal] = x;
    cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 
}


/*
*   move_right()
*   Inputs: none 
*   Return Value: none 
*	Function: moves screen_x and the cursor right:
*/
void 
move_right(void)
{
	if (screen_x[current_terminal] < NUM_COLS-1)
	{
		screen_x[current_terminal] ++; 
	}
	else if(screen_y[current_terminal] < NUM_ROWS-1)
	{
		screen_x[current_terminal] = 0;
		screen_y[current_terminal]++;
	}
	cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 
}

/*
*   void move_left()
*   Inputs: none 
*   Return Value: none 
*	Function: moves screen_x and the cursor left
*/
void
move_left(void)
{
	if (screen_x[current_terminal] > 0) 
	{
		screen_x[current_terminal] --; 
	}
	else if(screen_y[current_terminal] > 0)
	{
		screen_x[current_terminal] = NUM_COLS-1;
		screen_y[current_terminal]--;
	}
	cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 
}

/*
*   void cursor_loc(int x, int y) 
*   Inputs: int x, int y 
*   Return Value: none 
*	Function: moves the cursor to x,y 
*/
void cursor_loc(int x, int y)
{
	int coord = x + (y*NUM_COLS); 
	int coord2 = coord >> CHAR_W; 
	outb(C_LOC1, CURSOR_BIT);
	outb((unsigned char) (coord2 & MASK), CURSOR_OUT);
	outb(C_LOC2, CURSOR_BIT);
	outb((unsigned char) (coord & MASK), CURSOR_OUT );
} 
/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{
	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	while(*buf != '\0') {
		switch(*buf) {
			case '%':
				{
					int32_t alternate = 0;
					buf++;

format_char_switch:
					/* Conversion specifiers */
					switch(*buf) {
						/* Print a literal '%' character */
						case '%':
							putc('%');
							break;

						/* Use alternate formatting */
						case '#':
							alternate = 1;
							buf++;
							/* Yes, I know gotos are bad.  This is the
							 * most elegant and general way to do this,
							 * IMHO. */
							goto format_char_switch;

						/* Print a number in hexadecimal form */
						case 'x':
							{
								int8_t conv_buf[64];
								if(alternate == 0) {
									itoa(*((uint32_t *)esp), conv_buf, 16);
									puts(conv_buf);
								} else {
									int32_t starting_index;
									int32_t i;
									itoa(*((uint32_t *)esp), &conv_buf[8], 16);
									i = starting_index = strlen(&conv_buf[8]);
									while(i < 8) {
										conv_buf[i] = '0';
										i++;
									}
									puts(&conv_buf[starting_index]);
								}
								esp++;
							}
							break;

						/* Print a number in unsigned int form */
						case 'u':
							{
								int8_t conv_buf[36];
								itoa(*((uint32_t *)esp), conv_buf, 10);
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a number in signed int form */
						case 'd':
							{
								int8_t conv_buf[36];
								int32_t value = *((int32_t *)esp);
								if(value < 0) {
									conv_buf[0] = '-';
									itoa(-value, &conv_buf[1], 10);
								} else {
									itoa(value, conv_buf, 10);
								}
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a single character */
						case 'c':
							putc( (uint8_t) *((int32_t *)esp) );
							esp++;
							break;

						/* Print a NULL-terminated string */
						case 's':
							puts( *((int8_t **)esp) );
							esp++;
							break;

						default:
							break;
					}

				}
				break;

			default:
				putc(*buf);
				break;
		}
		buf++;
	}

	return (buf - format);
}

/*
* int32_t puts(int8_t* s);
*   Inputs: int_8* s = pointer to a string of characters
*   Return Value: Number of bytes written
*	Function: Output a string to the console 
*/

int32_t
puts(int8_t* s)
{
	register int32_t index = 0;
	while(s[index] != '\0') {
		putc(s[index]);
		index++;
	}

	return index;
}

/*
* void mt_putc(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the console of current process, which may not be the forground one
*/
void
mt_putc(uint8_t c)
{
	cli();
	if(current_active_process == current_terminal)
		return putc(c);

    if(c == '\n' || c == '\r') {
		screen_y[current_active_process]++;
		biggest_y[current_active_process]++;
        screen_x[current_active_process]=0;
    } else {
		
        uint8_t* cond = (uint8_t *)(map_video_mem + ((NUM_COLS*screen_y[current_active_process] + screen_x[current_active_process]) << 1));
		if( (int) cond > 0 && (int)cond < (int)map_video_mem+OFFSET_4K-1)
		{
        *(uint8_t *)(map_video_mem + ((NUM_COLS*screen_y[current_active_process] + screen_x[current_active_process]) << 1)) = c;
        *(uint8_t *)(map_video_mem + ((NUM_COLS*screen_y[current_active_process] + screen_x[current_active_process]) << 1) + 1) = attribs[current_active_process];
		}
        screen_x[current_active_process]++;
        //screen_x[current_terminal] %= NUM_COLS;
        //screen_y[current_terminal] = (screen_y[current_terminal] + (screen_x[current_terminal] / NUM_COLS)) % NUM_ROWS;
    }
    if(screen_x[current_terminal] >= NUM_COLS)
	{
		screen_y[current_active_process]++;
		biggest_y[current_active_process]++;
		screen_x[current_active_process] = 0;
	}
	if(screen_y[current_active_process] >= NUM_ROWS)
		mt_scroll();
	cursor_loc(screen_x[current_terminal], screen_y[current_terminal]);
	sti();
}
/*
* void putc(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the screen
*/
void
putc(uint8_t c)
{
	cli();
//	screen_x[current_terminal] = screen_x[current_terminal];
//	screen_y[current_terminal] = screen_y[current_terminal];
//	biggest_y[current_terminal] = biggest_y[current_terminal];

    if(c == '\n' || c == '\r') {
        screen_y[current_terminal]++;
		biggest_y[current_terminal]++;
        screen_x[current_terminal]=0;
    } else {
        uint8_t* cond = (uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1));
		if((int)cond > 0 && (int)cond < (int)video_mem+OFFSET_4K-1)
		{
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1) + 1) = attribs[current_terminal];
		}
        screen_x[current_terminal]++;
        //screen_x[current_terminal] %= NUM_COLS;
        //screen_y[current_terminal] = (screen_y[current_terminal] + (screen_x[current_terminal] / NUM_COLS)) % NUM_ROWS;
    }
    if(screen_x[current_terminal] >= NUM_COLS)
	{
		screen_y[current_terminal]++;
		biggest_y[current_terminal]++;
		screen_x[current_terminal] = 0;
	}
	if(screen_y[current_terminal] >= NUM_ROWS)
		scroll();
	cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 


	sti();
}

/*
* void putc_kb(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the screen and also wrap it + interpret
*	keyboard functionality like enter and backspace
*/
void
putc_kb(uint8_t c)
{
	uint8_t * cond;
 	//enter	
    if((c == '\n' || c == '\r')) {
		screen_y[current_terminal]++;
		screen_x[current_terminal] = 0;
    }
	//backspace
	else if( c == BS)
	{
    		if((screen_y[current_terminal] == 0 && screen_x[current_terminal] == 0))
	   		{
				//check if video address we are about to write to is valid
				cond = (uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1));
				if((int)cond > 0 && (int)cond < (int)video_mem+OFFSET_4K-1)
				{
					//write to vid mem
	   			*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1)) = ' ';
            	*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1) + 1) = attribs[current_terminal];
				}
	   		}
	   		else
	   		{
		   		screen_x[current_terminal]--; 
		   		if(screen_x[current_terminal] < 0)
		   		{
		   			screen_x[current_terminal] = NUM_COLS -1;
		   			screen_y[current_terminal]--;
					biggest_y[current_terminal]--;
		   			if(screen_y[current_terminal] < 0)
					{
		   				screen_y[current_terminal] = 0;
						biggest_y[current_terminal]=0;
					}
		   		}
				//check if video address we are about to write to is valid
				cond = (uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1));
				if((int)cond > 0 && (int)cond < (int)video_mem+OFFSET_4K-1)
				{
					//write to vid mem
					*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1)) = ' ';
					*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1) + 1) = attribs[current_terminal];
				}
        }
    	
    } 
	else
	{
    	if(screen_x[current_terminal] >= NUM_COLS)
		{
    		screen_y[current_terminal]++;
			biggest_y[current_terminal]++;
    		screen_x[current_terminal] = 0;
    	}
		//check if video address we are about to write to is valid
        cond = (uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1));
		if((int)cond > 0 && (int)cond < (int)video_mem+OFFSET_4K-1)
		{
			//write to vid mem
			*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1)) = c;
			*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + screen_x[current_terminal]) << 1) + 1) = attribs[current_terminal];
		}
        screen_x[current_terminal]++;
    }
	//if we've gone off the side of the screen, move to a new line
    if(screen_x[current_terminal] >= NUM_COLS)
	{
		screen_y[current_terminal]++;
		biggest_y[current_terminal]++;
		screen_x[current_terminal] = 0;
	}
	//if we've gone off the bottom of screen, scroll it
	if(screen_y[current_terminal] >= NUM_ROWS)
		scroll();
	cursor_loc(screen_x[current_terminal], screen_y[current_terminal]); 
}

/*
* void scoot_text();
*   Inputs: none 
*   Return Value: void
*	Function: scoot all text after cursor over to the right 
*/
void scoot_text()
{
	int x,y;
	for(y = NUM_ROWS -1; y > screen_y[current_terminal]; y--)
	{
			*(uint8_t *)(video_mem + ((NUM_COLS*y + 0) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*(y-1) + NUM_COLS -1) << 1));
			*(uint8_t *)(video_mem + ((NUM_COLS*y + 0) << 1) + 1) = attribs[current_terminal];

		for(x = NUM_COLS -1; x > 0; x--)
		{
			*(uint8_t *)(video_mem + ((NUM_COLS*y + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*y + x -1) << 1));
			*(uint8_t *)(video_mem + ((NUM_COLS*y + x) << 1) + 1) = attribs[current_terminal];
		}
	}
	for(x = NUM_COLS -1; x > screen_x[current_terminal]; x--)
	{
		*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + x-1) << 1));
		*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[current_terminal] + x) << 1) + 1) = attribs[current_terminal];
	}
}

/*
* void mt_scroll();
*   Inputs: none 
*   Return Value: void
*	Function: scroll the screen up. can be foreground our background
*/
void 
mt_scroll()
{
	if (current_active_process == current_terminal) return scroll();
	int x, y;
	for(y = 1; y < NUM_ROWS; ++y)
	{
		for(x = 0; x < NUM_COLS; x++)
		{
			//write to vid mem
			*(uint8_t *)(map_video_mem + ((NUM_COLS*(y-1) + x) << 1)) = *(uint8_t *)(map_video_mem + ((NUM_COLS*(y) + x) << 1)) ;
			*(uint8_t *)(map_video_mem + ((NUM_COLS*(y-1) + x) << 1)+1) = *(uint8_t *)(map_video_mem + ((NUM_COLS*(y) + x) << 1)+1) ;

		}
	}
	screen_y[current_active_process] = biggest_y[current_active_process] = NUM_ROWS -1;
	for(x = 0; x < NUM_COLS; x++)
	{
			//write to vid mem
			*(uint8_t *)(map_video_mem + ((NUM_COLS*(screen_y[current_active_process]) + x) << 1)) = 0x00;
			*(uint8_t *)(map_video_mem + ((NUM_COLS*(screen_y[current_active_process]) + x) << 1)+1) = attribs[current_active_process];

	}
	screen_x[current_active_process] = 0;
}
/*
* void scroll();
*   Inputs: none 
*   Return Value: void
*	Function: scroll the screen up. 
*/
void 
scroll()
{
	int x, y;
	for(y = 1; y < NUM_ROWS; ++y)
	{
		for(x = 0; x < NUM_COLS; x++)
		{
			//write to vid mem
			*(uint8_t *)(video_mem + ((NUM_COLS*(y-1) + x) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*(y) + x) << 1)) ;
			*(uint8_t *)(video_mem + ((NUM_COLS*(y-1) + x) << 1)+1) = *(uint8_t *)(video_mem + ((NUM_COLS*(y) + x) << 1)+1) ;

		}
	}
	screen_y[current_terminal] = biggest_y[current_terminal] = NUM_ROWS -1;
	for(x = 0; x < NUM_COLS; x++)
	{
			//write to vid mem
			*(uint8_t *)(video_mem + ((NUM_COLS*(screen_y[current_terminal]) + x) << 1)) = 0x00;
			*(uint8_t *)(video_mem + ((NUM_COLS*(screen_y[current_terminal]) + x) << 1)+1) = attribs[current_terminal];

	}
	screen_x[current_terminal] = 0;
	write_y[current_terminal]--;
	//write_x[current_terminal] = 0;
}
/*
* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
*   Inputs: uint32_t value = number to convert
*			int8_t* buf = allocated buffer to place string in
*			int32_t radix = base system. hex, oct, dec, etc.
*   Return Value: number of bytes written
*	Function: Convert a number to its ASCII representation, with base "radix"
*/

int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
	static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int8_t *newbuf = buf;
	int32_t i;
	uint32_t newval = value;

	/* Special case for zero */
	if(value == 0) {
		buf[0]='0';
		buf[1]='\0';
		return buf;
	}

	/* Go through the number one place value at a time, and add the
	 * correct digit to "newbuf".  We actually add characters to the
	 * ASCII string from lowest place value to highest, which is the
	 * opposite of how the number should be printed.  We'll reverse the
	 * characters later. */
	while(newval > 0) {
		i = newval % radix;
		*newbuf = lookup[i];
		newbuf++;
		newval /= radix;
	}

	/* Add a terminating NULL */
	*newbuf = '\0';

	/* Reverse the string and return */
	return strrev(buf);
}

/*
* int8_t* strrev(int8_t* s);
*   Inputs: int8_t* s = string to reverse
*   Return Value: reversed string
*	Function: reverses a string s
*/

int8_t*
strrev(int8_t* s)
{
	register int8_t tmp;
	register int32_t beg=0;
	register int32_t end=strlen(s) - 1;

	while(beg < end) {
		tmp = s[end];
		s[end] = s[beg];
		s[beg] = tmp;
		beg++;
		end--;
	}

	return s;
}

/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlen(const int8_t* s)
{
	register uint32_t len = 0;
	while(s[len] != '\0')
		len++;

	return len;
}

/*
* void* memset(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive bytes of pointer s to value c
*/

void*
memset(void* s, int32_t c, uint32_t n)
{
	c &= 0xFF;
	asm volatile("                  \n\
			.memset_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memset_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memset_aligned \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memset_top     \n\
			.memset_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     stosl           \n\
			.memset_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memset_done    \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%edx       \n\
			jmp     .memset_bottom  \n\
			.memset_done:           \n\
			"
			:
			: "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_word(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set lower 16 bits of n consecutive memory locations of pointer s to value c
*/

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosw           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_dword(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive memory locations of pointer s to value c
*/

void*
memset_dword(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosl           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memcpy(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of copy
*			const void* src = source of copy
*			uint32_t n = number of byets to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of src to dest
*/

void*
memcpy(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			.memcpy_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memcpy_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memcpy_aligned \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memcpy_top     \n\
			.memcpy_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     movsl           \n\
			.memcpy_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memcpy_done    \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%edx       \n\
			jmp     .memcpy_bottom  \n\
			.memcpy_done:           \n\
			"
			:
			: "S"(src), "D"(dest), "c"(n)
			: "eax", "edx", "memory", "cc"
			);

	return dest;
}

/*
* void* memmove(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of move
*			const void* src = source of move
*			uint32_t n = number of byets to move
*   Return Value: pointer to dest
*	Function: move n bytes of src to dest
*/

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			cmp     %%edi, %%esi    \n\
			jae     .memmove_go     \n\
			leal    -1(%%esi, %%ecx), %%esi    \n\
			leal    -1(%%edi, %%ecx), %%edi    \n\
			std                     \n\
			.memmove_go:            \n\
			rep     movsb           \n\
			"
			:
			: "D"(dest), "S"(src), "c"(n)
			: "edx", "memory", "cc"
			);

	return dest;
}

/*
* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*			uint32_t n = number of bytes to compare
*	Return Value: A zero value indicates that the characters compared 
*					in both strings form the same string.
*				A value greater than zero indicates that the first 
*					character that does not match has a greater value 
*					in str1 than in str2; And a value less than zero 
*					indicates the opposite.
*	Function: compares string 1 and string 2 for equality
*/

int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for(i=0; i<n; i++) {
		if( (s1[i] != s2[i]) ||
				(s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string
*/

int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
	int32_t i=0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
	int32_t i=0;
	while(src[i] != '\0' && i < n) {
		dest[i] = src[i];
		i++;
	}

	while(i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}

/*
* void test_interrupts(void)
*   Inputs: void
*   Return Value: void
*	Function: increments video memory. To be used to test rtc
*/

void
test_interrupts(void)
{
	int32_t i;
	for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
		video_mem[i<<1]++;
	}
}



