#ifndef _MY_STDIO_H
#define _MY_STDIO_H

void clear_screen(void);
void puts(char *text);
void putch(unsigned char c);

/**
 * Print to standart output stream.
 * @param format    Format string to be output
 * @param ...       Arguments corresponding to conversion specifiers in format
 * @return          Number of characters printed
 */
int printf(const char *fmt, ...);

#endif
