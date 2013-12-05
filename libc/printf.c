#include <string.h>
#include <stdio.h>
#include <syscall.h>

int printf(const char *format, ...){
	char *buffer = (char*) malloc(BUF_SIZE);
	char *buffer_fmt = (char*)malloc(strlen(format)+1);
	memcpy(buffer_fmt, format, strlen(format)+1);
    __builtin_va_list ap;
	int num_char;
	/* prepare valist from arguments */
    __builtin_va_start(ap, format);
	num_char = sprintf_va(buffer, buffer_fmt,&ap);	
	/* Print onto the console */
	buffer[num_char] = '\0';
	write(STDOUT, buffer, strlen(buffer));
//	uprintf(buffer);
    return num_char;

}
