#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H
#include <defs.h>
//TODO: Intuitve add comments
int terminal_open(const char* pathname);
uint64_t terminal_read(int fd, void *buf, uint64_t count);
uint64_t terminal_write(int fd, void *buf, uint64_t count);
int terminal_close(int fd);
/* Enqueue a character to the terminal buffer */
void terminal_enqueue(char input_char);
#endif

