#include <stdio.h>
#include <syscall.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {
	int ret;
	int64_t sleep_interval = atoi(argv[1]);
	printf("\nSleeping for %d milli seconds\n",sleep_interval);
	ret = sleep(sleep_interval);
	if(ret != 0){
		printf("Invalid sleep interval %d\n",sleep_interval);
		return ret;
	}
	return 0;
}
