#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>

/* A dummy interpreter used for testing the sh command */
int main(int argc, char* argv[], char* envp[]) {
	printf("::Echo interpreter::\n");
	for(int count=0; count<argc; count++){
		printf("Arg received [%d]:%s\n", count, argv[count]);
	}
	return 0;
}
