#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>

/* This program prints World 5 times at an interval of 1 second */
int main(int argc, char* argv[], char* envp[]) {
	int counter = 3;
	printf("ENV: %s",envp[0]);
	while(counter--){
		sleep(1000);
		printf("[%d] World!\n",get_pid());
	}	
	return 0;
}
