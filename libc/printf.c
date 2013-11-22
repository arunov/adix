#include <syscall.h>
int printf(const char *format, ...) {
	uprintf(format);
	return 0;
}
