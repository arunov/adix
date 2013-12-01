#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
int main(int argc, char* argv[], char* envp[]) {
	int counter = 0;
    uint64_t child_pid = fork();
    printf("[pid: %p] fork returned: %p\n", get_pid(), child_pid);

	if(child_pid == 0){
		execvpe("bin/world",NULL,NULL);		
	}else{
		wait_pid(child_pid);
	}

	while(counter++ < 5){
		printf("[pid: %p] Hello number %d\n", get_pid(), counter);
	}

	return 0;
}
