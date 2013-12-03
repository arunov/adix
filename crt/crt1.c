#include <stdlib.h>
#include <syscall.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>

void _start(uint64_t argc, char **argv, char **envp) {
	int res =0;
	res = main(argc, argv, envp);
	exit(res);
}
