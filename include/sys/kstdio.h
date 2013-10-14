#ifndef _K_STDIO_H
#define _K_STDIO_H

#define VIDEO_MEMORY_ADDRESS 0xB8000UL
void *global_video_vaddr;

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
