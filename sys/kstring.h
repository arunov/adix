#ifndef _KSTRING_H
#define _KSTRING_H

void *memcpy(void *dest, const void *src, unsigned int count);
void *memset(void *dest, char val, unsigned int count);
unsigned short *memsetw(unsigned short *dest, unsigned short val, unsigned int count);
unsigned int strlen(const char *str);

#endif
