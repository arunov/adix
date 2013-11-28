#include <stdlib.h>
#include <syscall.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>

void _start(uint64_t argc, char **argv, char **envp) {
/*	int argc = 1;
	char* argv[10];
	char* envp[10];*/
	printf("\nNumber of args: %d",argc);
	for(int i=0;i<argc;i++){
		printf("\nArg nu %d is : %s ", i, argv[i]);
	}
	int res =0;
	res = main(argc, argv, envp);
	exit(res);
}
