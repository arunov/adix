#include <stdlib.h>
#include <syscall.h>
#include <defs.h>
#include <stdio.h>
#include <string.h>

void _start(uint64_t argc, char **argv, char **envp) {
/*	int argc = 1;
	char* argv[10];
	char* envp[10];*/
	printf("Number of args: %d\n",argc);
	for(int i=0;i<argc;i++){
		printf("Arg nu %d is : %s\n", i, argv[i]);
	}
	int res =0;
	res = main(argc, argv, envp);
	exit(res);
}
