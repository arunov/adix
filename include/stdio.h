#ifndef _STDIO_H
#define _STDIO_H

#include<defs.h>

#define STDIN 0
#define STDOUT 1
#define BUF_SIZE 1024
#define NULL 0
int printf(const char *format, ...);
int scanf(char const *buf, void *addr);
#endif
