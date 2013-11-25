#ifndef _TERMINAL_DRIVER_H
#define _TERMINAL_DRIVER_H

#define FLUSH_CHAR '\n'
#include <defs.h>
#include <sys/filesystems/file_structures.h>
//TODO: Intuitve add comments
int terminal_open(const char* pathname);
int64_t terminal_read(int fd, void *buf, uint64_t count);
int64_t terminal_write(int fd, void *buf, uint64_t count);
int terminal_close(int fd);
/* Enqueue a character to the terminal buffer */
void terminal_enqueue(char input_char);

extern struct operation_pointers terminal_ops;

struct operation_pointers* get_terminal_ops();

#endif

