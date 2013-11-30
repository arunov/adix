#include <sys/kstdarg.h>
#include <sys/kstdio.h>
#include <sys/sysconf.h>
#include <string.h>

#define MAX_LENGTH_OF_STR_INT       11  // length of 32-bit uint max 4294967296
                                        //  (10) + '-' (1)

#define DECIMAL_BASE    10
#define HEX_BASE        16

/* States of format string parser of printf */
enum {
    PRINTF_ST_NORMAL = 0,       // 0
    PRINTF_ST_PERCENT,          // 1
    PRINTF_ST_NUM_OF_STATES     // 3
};

/*
 * Print 'bytes' bytes of value 'parg' in hexadecimal 
 * @param bytes    Number of bytes to be printed
 * @param parg     Pointer to data that needs to be printed
 * @return         Number of characters printed
 */
static int hex_to_string(const void *parg, int bytes) {
    int nib_i, num_char = 0;
    unsigned char c;
    int start_print = 0; // Used for ignoring leading zeros

    puts("0x");
    num_char += 2;

#ifdef __LITTLE_ENDIAN__
    for(nib_i = (bytes * 2) - 1; nib_i >= 0; nib_i --) {
        if(nib_i % 2 == 0)
#else
    for(nib_i = 0; nib_i < (bytes * 2); nib_i ++) {
        if(nib_i % 2)
#endif
        {
            c = (*((char*)parg + nib_i/2)) & 0x0F;
        } else {
            c = ((*((char*)parg + nib_i/2)) & 0xF0) >> 4;
        }

        if(0 != c) {
            start_print = 1;
        }

        if(c < DECIMAL_BASE) {
            c += '0';
        } else {
            c += 'a' - DECIMAL_BASE;
        }

        if(start_print) {
            num_char++;
            putch(c);
        }
    }

    /* If hexadecimal value is 0, then it needs to be printed explicitly.
       All leading zeros are ignored. */
    if(2 == num_char) {
        num_char++;
        putch('0');
    }

    return num_char;
}

/*
 * Print pointer
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int pointer_to_string(va_list *ap) {
    void *ptr = va_arg(*ap, void*);
    return hex_to_string((void*)&ptr, sizeof(void*));
}

/*
 * Print hexadecimal
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int hexnum_to_string(va_list *ap) {
    int d = va_arg(*ap, int);
    return hex_to_string(&d, sizeof(int));
}

/*
 * Print integer
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int int_to_string(va_list *ap) {
    int d = va_arg(*ap, int);
    int neg = 0;
    char printf_string_buff[MAX_LENGTH_OF_STR_INT+1];    //to accommodate '\0' 
    char *loc = printf_string_buff + sizeof(printf_string_buff)-1;
    *loc = '\0';

    if(0 > d) {
        neg = 1;
        d *= -1;
    }

    do {
        loc --;
        *loc = (d % DECIMAL_BASE) + '0';
        d /= DECIMAL_BASE;
    } while(d > 0);

    if(neg) {
        loc --;
        *loc = '-';
    }

    puts(loc);
    return printf_string_buff + sizeof(printf_string_buff) - loc;
}

/*
 * Print character
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int char_to_string(va_list *ap) {
    putch((char) va_arg(*ap, int));
    return 1;
}

/*
 * Print string
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int string_to_string(va_list *ap) {
    char *s = va_arg(*ap, char*);
    puts(s);
    return strlen(s);
}

/*
 * Print percent
 * @param ap    Pointer to variable argument list
 * @return      Number of characters printed
 */
static int percent_to_string(va_list *ap) {
    putch('%');
    return 1;
}

/* Structure for parsing format specifiers */
typedef struct format_string_parse_helper {

    /* Format specifier */
    const char conv_spec;

    /* Function pointer to handle format specifier */
    int (*conv_to_string)(va_list*);
} format_string_parser;

/* Format specifiers and handlers */
static format_string_parser format_str_conv[] = {
    {'d', int_to_string},       // decimal
    {'i', int_to_string},       // decimal
    {'c', char_to_string},      // char
    {'s', string_to_string},    // string
    {'x', hexnum_to_string},    // hexadecimal
    {'X', hexnum_to_string},    // hexadecimal
    {'p', pointer_to_string},   // pointer
    {'%', percent_to_string},   // percent
    {'\0', 0},                  // end of string
};

/* See kstdio.h */
int printf(const char *format, ...) {

    const char *l_format = format;

    va_list ap;
    char c;
    int conv_spec_i, num_char = 0;

    int state = PRINTF_ST_NORMAL;

    va_start(ap, format);

    while (*l_format) {
        c = *l_format++;

        if(PRINTF_ST_PERCENT == state) {

            for(conv_spec_i = 0;
                conv_spec_i < ((sizeof(format_str_conv))/(sizeof(format_string_parser)));
                conv_spec_i ++) {

                if(format_str_conv[conv_spec_i].conv_spec == c) {
                    if(format_str_conv[conv_spec_i].conv_to_string) {
                        num_char += format_str_conv[conv_spec_i].conv_to_string(&ap);
                    }
                }
            }
 
            state = PRINTF_ST_NORMAL;

        } else if ('%' == c) {
            state = PRINTF_ST_PERCENT;
            continue;
        } else { // print to output
            putch(c);
            num_char ++;
        }
    }

    va_end(ap);
    return num_char;
}

