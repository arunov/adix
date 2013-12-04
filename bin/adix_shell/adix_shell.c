#include <stdio.h>
#include <string.h>
#include <syscall.h>
#define BUF_SIZE 1024
#define DELIM " "
#define BG_SYMBOL "bg"
int parse_shell_command_args(char *buffer, char *child_argv[]){

	int len = 0;
	int i = 0;
	for (i=0; *buffer != NULL; i++){
			/* This strtok does not rememeber where it left last time.
			So remember here where it stopped last time */
			child_argv[i] = strtok(buffer,DELIM);
			len = strlen(child_argv[i]);
			buffer += len+1;
	}
	child_argv[i] = NULL;
	return i;
}
int is_foreground(int argc, char *child_argv[]){
	return str_equal(child_argv[argc-1],BG_SYMBOL) ? 0 : 1;
}

void init_shell(){
	clrscr();
}

char* resolve_path(char *command){
	return command;
}

char** get_env(){
	return NULL;//TODO
}

void run_shell(){
	uint64_t pid = 0;
	int child_argc;
	int foreground;
	char *buffer = malloc(BUF_SIZE);//each process should get its own buffer
	char **child_argv = (char**) malloc(10 * sizeof(char*));
	int count = read(STDIN, buffer, BUF_SIZE);
	write(STDOUT, buffer, count);
	buffer[count] = '\0';
	child_argc = parse_shell_command_args((char*)buffer, child_argv);
	foreground = is_foreground(child_argc, child_argv);
//	printf("Is foreground: %d\n",foreground);
	pid = fork();
	if(pid == 0){
		execvpe(child_argv[0], child_argv, get_env());
		printf("Execution of the process \"%s\" failed",child_argv[0]);
		exit(0);//If execvpe returned => it failed
	} else{
		if(foreground){
			wait_pid(pid);
		}
	}
}

int main(int argc, char* argv[], char* envp[]) {
	init_shell();
	int count = 1;
	while(1){
		printf("\n##ADIX[%d]>> ",count);
		count++;
		run_shell();
	}
	exit(0);
}
