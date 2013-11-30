#ifndef _STRING_H
#define _STRING_H

unsigned char inportb (unsigned short port);
void outportb (unsigned short port, unsigned char data);


void *memcpy(void *dest, const void *src, unsigned int count);
void *memset(void *dest, char val, unsigned int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, unsigned int count);
unsigned int strlen(const char *str);

int sprintf_va(char *buf, const char *format, __builtin_va_list *ap);
int sprintf(char *buf, const char *format, ...);
/* Returns pointer to the string tokenized at the first occurrence of the
 *  delimiter. This method modifies the string passed as argument */
char* strtok(register char *s, char *delim);
/* Compares two strings. Returns 0 if they are equal, 1 otherwise*/
int str_equal(char *src, char *dst);
#endif
