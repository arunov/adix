#include<string.h>
#include<stdio.h>
#include<syscall.h>
/*
int scanf(const char *format, ...){
	char *buffer = (char *)malloc(BUF_SIZE);
	__builtin_va_list ap;
	int ret;

	__builtin_va_start(ap, format);
	ret = sscanf(buffer, format, &ap);
	__builtin_va_end(ap);
	return ret;

}

int sscanf(char *buf, const char* format, __builtin_va_list *ap){
	int ret;
	retun ret;

}
*/

int scanf(char const *inp, void* addr){
	int c;
	char *buffer = (char *)malloc(BUF_SIZE);
	int ret = 0;

	c = *inp++;
	if(c != '%'){
		printf("scanf fails: scanf takes %%x %%d %%s");
		return ret;
	}

	c = *inp++;
	switch (c){
		case 'd':{
			read(STDIN, buffer,BUF_SIZE );
			int scanned_int;
			scanned_int = atoi(buffer);
			int *address = (int *)addr;
			*address = scanned_int;
			ret = 1;
			break;
			}
		case 's':{
			int read_count = read(STDIN, buffer,BUF_SIZE );
			memcpy(addr, (void *)buffer, read_count);
			ret = 1;
			break;
			}
		case 'x':{
			read(STDIN, buffer,BUF_SIZE );
			int scanned_hex;
			scanned_hex = atohex(buffer);
			int *address = (int *)addr;
			*address = scanned_hex;
			ret = 1;
			break;
			}
		default :
			break;

	}
	return ret;
}

