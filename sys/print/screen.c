#include <string.h>
#include <sys/kstdio.h>

#define BUFFER_ADDRESS(CURSOR_X, CURSOR_Y) (unsigned int)(CURSOR_Y * 80 + CURSOR_X);
#define GET_COLOUR(FG, BG) ((0x0F & FG) | ((0x0F & BG) << 4));

static unsigned int global_cursor_attribute = 0x0F;
static unsigned int global_cursor_x, global_cursor_y;

unsigned char inportb (unsigned short _port)
{
	unsigned char rv;
	__asm volatile("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

void outportb (unsigned short _port, unsigned char _data)
{
	__asm volatile ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

void blink_cursor(unsigned int pos_x, unsigned int pos_y)
{
	unsigned int pos_memory = BUFFER_ADDRESS(pos_x, pos_y);
	
	outportb(0x3D4, 14);
	outportb(0x3D5, pos_memory >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, pos_memory);
}

/* Scrolls the screen */
void scroll_screen(void)
{
	unsigned int blank, temp;
	unsigned short *mem_ptr;

	/* A blank is defined as a space... we need to give it
	*  backcolor too */
	blank = 0x20 | (global_cursor_attribute << 8);

	/* Row 25 is the end, this means we need to scroll up */
 	if(global_cursor_y >= 24)
 	{
        	/* Move the current text chunk that makes up the screen
        	*  back in the buffer by a line */
        	temp = global_cursor_y - 24 + 1;
		mem_ptr = (unsigned short *)global_video_vaddr;
        	memcpy (mem_ptr, mem_ptr + temp * 80, (25 - temp) * 80 * 2);

        	/* Finally, we set the chunk of memory that occupies
        	*  the last line of text to our 'blank' character */
        	memsetw (mem_ptr + (24 - temp) * 80, blank, 80);
        	global_cursor_y = 24 - 1;
    	}
}

/* Updates the hardware cursor: the little blinking line
*  on the screen under the last character pressed! */
void move_cursor(void)
{
	 blink_cursor(global_cursor_x, global_cursor_y);
}

/* Clears the screen */
void clear_screen()
{
    	unsigned int blank;
	unsigned short *mem_ptr = (unsigned short *)global_video_vaddr;
    	int i;

    	/* Again, we need the 'short' that will be used to
    	*  represent a space with color */
    	blank = 0x20 | (global_cursor_attribute << 8);

    	/* Sets the entire screen to spaces in our current
    	*  color */
    	for(i = 0; i < 25; i++)
        	memsetw (mem_ptr + i * 80, blank, 80);

    	/* Update out virtual cursor, and then move the
    	*  hardware cursor */
    	global_cursor_x = 0;
    	global_cursor_y = 0;
    	//blink_cursor(global_cursor_x, global_cursor_y);
}

/* Puts a single character on the screen */
void putch(unsigned char c)
{
    	unsigned short *where;
	unsigned short *textmemptr = (unsigned short *)global_video_vaddr;
    	unsigned att = global_cursor_attribute << 8;

    	/* Handle a backspace, by moving the cursor back one space */
    	if(c == 0x08)
    	{
        	if(global_cursor_x != 0) global_cursor_x--;
    	}
    	/* Handles a tab by incrementing the cursor's x, but only
    	*  to a point that will make it divisible by 8 */
    	else if(c == 0x09)
    	{
        	global_cursor_x = (global_cursor_x + 8) & ~(8 - 1);
    	}
    	/* Handles a 'Carriage Return', which simply brings the
    	*  cursor back to the margin */
    	else if(c == '\r')
    	{
        	global_cursor_x = 0;
    	}
    	/* We handle our newlines the way DOS and the BIOS do: we
    	*  treat it as if a 'CR' was also there, so we bring the
    	*  cursor to the margin and we increment the 'y' value */
    	else if(c == '\n')
    	{
        	global_cursor_x = 0;
        	global_cursor_y++;
    	}
    	/* Any character greater than and including a space, is a
    	*  printable character. The equation for finding the index
    	*  in a linear chunk of memory can be represented by:
    	*  Index = [(y * width) + x] */
    	else if(c >= ' ')
    	{
        	where = textmemptr + (global_cursor_y * 80 + global_cursor_x);
        	*where = c | att;	/* Character AND attributes: color */
        	global_cursor_x++;
    	}

    	/* If the cursor has reached the edge of the screen's width, we
    	*  insert a new line in there */
    	if(global_cursor_x >= 80)
    	{
        	global_cursor_x = 0;
        	global_cursor_y++;
    	}

    	/* Scroll the screen if needed, and finally move the cursor */
    	scroll_screen();
    	move_cursor();
}

/* Uses the above routine to output a string... */
void puts(char *text)
{
    	int i;

   	for (i = 0; i < strlen(text); i++)
    	{
        	putch(text[i]);
    	}
}

