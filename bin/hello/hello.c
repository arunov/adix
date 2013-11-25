#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
int main(int argc, char* argv[], char* envp[]) {
	int counter = 0;
	while(counter++ < 5){
		write(STDOUT,"Hello",6);
		yield();
	}
	return 0;
}
