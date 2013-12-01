#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>
int main(int argc, char* argv[], char* envp[]) {
	uint64_t wcounter =1 ;
	int counter = 5;
	while(counter){
		if(wcounter++ % 50000000 == 0){
			counter--;
			printf("World!\n");
		}
	}
	return 0;
}
