#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>

/* This program tests stdin/out/err etc */
int main(int argc, char* argv[], char* envp[]) {
	char buffer[100];
	char stream[] = "Error stream";
	//char* buffer = (char*)malloc(100);
	printf("Type something interesting!\n");
	read(STDIN, buffer,100);
	write(STDOUT, buffer, 100);
	/* At present writes onto the Terminal - will be changed
	 * once redirection and filesystems are introduced */
	write(STDERR, stream, 100);
	return 0;
}
