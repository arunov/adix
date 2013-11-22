#include <stdlib.h>
#include <syscall.h>
void _start(void) {
	int argc = 1;
	char* argv[10];
	char* envp[10];
	int res;
	res = main(argc, argv, envp);
	exit(res);
}
