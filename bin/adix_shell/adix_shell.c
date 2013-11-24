#include <stdio.h>
#include <syscall.h>
#define BUF_SIZE 1024
char buffer[BUF_SIZE];

int parse_shell_command_args(char *child_argv[]){
	return 0;
}
int is_foreground(int argc, char *child_agv[]){
	return 0;
}
void wait_pid(uint64_t pid){
	return ;
}

void init_shell(){

}

int main(int argc, char* argv[], char* envp[]) {
	init_shell();
	uint64_t pid = 0;
	int child_argc;
	char *child_argv[10];
	int foreground;
	while(1){
		int count = read(STDIN, &buffer, BUF_SIZE);
		write(STDOUT, &buffer, count);

		child_argc = parse_shell_command_args(child_argv);
		foreground = is_foreground(child_argc, child_argv);
		//fork()	
		if(pid == 0){
//			execvpe(child_argv[0], child_argc, child_argv); 
				
		}else{
			if(foreground){
				wait_pid(pid);
			}
		}

	}
	
}
