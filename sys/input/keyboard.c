#include <defs.h>
#include <sys/irq.h>
#include <sys/kstring.h>
#include <sys/kstdio.h>
#include <sys/keyboard.h>
#include <sys/utils.h>
#include <sys/terminal/terminal_driver.h>
struct kbkeypress {
    uint64_t keys[2];
} gKbKeyState = {{0, 0}};

int kbCaps = 0;

unsigned char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

unsigned char shift_kbdus[128] =
{
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
  '(', ')', '_', '+', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     /* 39 */
 '\"', '~',   0,                /* Left shift */
 '|', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/*struct multiKeyMapAttr {
    uint64_t key[2];
    char *print;
} multiKeyMap = {
    { {0x00000000, 0x00000000}, "<CTRL+"},
}*/

void keyboard_handler()
{
    unsigned char scancode;
    char *video_buf = (char *)(((uint64_t)global_video_vaddr) + 4000) ;
    int key_scancode;

    /* Read from the keyboard's data buffer */
    scancode = inportb(0x60);
    key_scancode = scancode & 0x7f;

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys... */
        gKbKeyState.keys[key_scancode/64] &= (0xfffffffffffffffeUL << (key_scancode%64));
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */

        gKbKeyState.keys[key_scancode/64] |= (0x1UL << (key_scancode%64));

        int bitsSet = numOfBitsSet(&gKbKeyState, 16);

        #if DEBUG
            printf("Bits Set: %d\n", bitsSet);
            printf("Scan Code : %d\n", (int)scancode);
        #endif
   
        char c1 = ' ';
        char c2 = ' ';
        if(bitsSet == 1)
        {
            // Single key pressed
            c2 = kbdus[scancode];
        } else {
            uint64_t shift_pressed = ((gKbKeyState.keys[key_scancode/64] >> 42) & 0x1UL);
            uint64_t control_pressed = ((gKbKeyState.keys[key_scancode/64] >> 29) & 0x1UL);
            
            #if DEBUG
                printf("Shift: %d, Control: %d\n",shift_pressed, control_pressed);
            #endif

            if(shift_pressed == 0x1UL)
            {
                c2 = shift_kbdus[scancode];
            }
            else if(control_pressed == 0x1UL)
            {
                c1 = '^';
                c2 = shift_kbdus[scancode];
            }
        }
	terminal_enqueue(c2);
        if(c2 != '\n' && c2 != '\t')
        {
            *(video_buf-20) = c1;
            *(video_buf-18) = c2;
        }
    }
}

/* Sets up the system clock by installing the keyboard handler
*  into IRQ1 */
void keyboard_install()
{
    /* Installs 'keyboard_handler' to IRQ1 */
    irq_install_handler(1, keyboard_handler);
}


