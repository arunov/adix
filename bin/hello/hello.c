#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
int main(int argc, char* argv[], char* envp[]) {
	printf("Entered main");
	int counter = 0;
	int64_t pid_ret;
    int64_t child_pid = fork();
	if(child_pid){
		//Fork only in parent
    	child_pid = fork();
	}
	if(child_pid){
		//Fork only in parent
    	child_pid = fork();
	}

	if(child_pid){
		//Fork only in parent
    	child_pid = fork();
	}
    printf("[pid: %p] fork returned: %p\n", get_pid(), child_pid);

	if(child_pid == 0){
		execvpe("bin/world",NULL,envp);		
	}else{
		pid_ret = wait();
		printf("Parent exiting with returned PID [%d]\n",pid_ret);
	}

	while(counter++ < 2){
		printf("[pid: %p] Hello number %d\n", get_pid(), counter);
	}

	return 0;
}
