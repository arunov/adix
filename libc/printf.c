#include <string.h>
#include <stdio.h>
#include <syscall.h>

int printf(const char *format, ...){
	char *buffer = (char*) malloc(BUF_SIZE);
    __builtin_va_list ap;
	int num_char;
	/* prepare valist from arguments */
    __builtin_va_start(ap, format);
	num_char = sprintf_va(buffer, format,&ap);	
	/* Print onto the console */
	buffer[num_char] = '\0';
	uprintf(buffer);
    return num_char;

}
