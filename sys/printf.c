#include "include/kstdarg.h"
#include "kstdio.h"
#include "include/sysconf.h"
#include "kstring.h"

#define MAX_LENGTH_OF_STR_INT       12  // length of 32-bit uint max 4294967296 (10)
                                        // + '-' (1) + '\0' (1)
#define MAX_LENGTH_OF_STR_POINTER   17  // length of 64-bit hex (16) + '\0' (1)

#define DECIMAL_BASE    10
#define HEX_BASE        16

/* States of format string parser of printf */
enum {
    PRINTF_ST_NORMAL = 0,       // 0
    PRINTF_ST_PERCENT,          // 1
    PRINTF_ST_NUM_OF_STATES     // 3
};

static int hex_to_string(const void *parg, int bytes) {
    int nib_i, num_char = 0;
    unsigned char c;
    int start_print = 0; // Used for ignoring trailing zeros

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

        if(0 != c && 0 == start_print) {
            start_print = 1;
        }

        if(c < DECIMAL_BASE) {
            c += '0';
        } else {
            c += 'A' - DECIMAL_BASE;
        }

        if(start_print) {
            num_char++;
            putch(c);
        }
    }

    return num_char;
}

static int long_long_int_to_string(long long int d) {
    
    int neg = 0;
    char printf_string_buff[2*MAX_LENGTH_OF_STR_INT];
    char *loc = printf_string_buff + sizeof(printf_string_buff);
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

static int pointer_to_string(va_list *ap) {
    void *ptr = va_arg(*ap, void*);
    //return hex_to_string(ptr, POINTER_BYTES);
    return long_long_int_to_string((long long int)ptr);
}

static int hexnum_to_string(va_list *ap) {
    int d = va_arg(*ap, int);
    return hex_to_string(&d, DECIMAL_BYTES);
}

static int int_to_string(va_list *ap) {
    int d = va_arg(*ap, int);
    int neg = 0;
    char printf_string_buff[MAX_LENGTH_OF_STR_INT];
    char *loc = printf_string_buff + sizeof(printf_string_buff);
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

static int char_to_string(va_list *ap) {
    putch((char) va_arg(*ap, int));
    return 1;
}

static int string_to_string(va_list *ap) {
    char *s = va_arg(*ap, char*);
    puts(s);
    return strlen(s);
}

static int percent_to_string(va_list *ap) {
    putch('%');
    return 1;
}

typedef struct format_string_parse_helper {
    const char conv_spec;
    int (*conv_to_string)(va_list*);
} format_string_parser;

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

int printf(const char *format, ...) {

    va_list ap;
    char c;
    int conv_spec_i, num_char = 0;

    int state = PRINTF_ST_NORMAL;

    va_start(ap, format);

    while (*format) {
        c = *format++;

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

