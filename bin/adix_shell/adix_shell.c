#include <stdio.h>
#include <string.h>
#include <syscall.h>
#define BUF_SIZE 1024
#define DELIM " "
#define BG_SYMBOL "bg"
char buffer[BUF_SIZE];

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
void wait_pid(uint64_t pid){
	return ;
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
	char *child_argv[10];
	int foreground;
    char *x = malloc(500);
    *x = 'a';
    *(x+1) = 'b';
    *(x+2) = '\0';
    printf("x: %s\n", x);
	int count = read(STDIN, &buffer, BUF_SIZE);
	printf("\nCommand you entered is %d character long and is:",count);
	write(STDOUT, &buffer, count);

	child_argc = parse_shell_command_args((char*)buffer, child_argv);
	foreground = is_foreground(child_argc, child_argv);
	printf("\nNumbe of args %d",child_argc);
	printf("\nIs foreground: %d",foreground);

	//fork()	
	if(pid == 0){
		execvpe(child_argv[0], child_argv, get_env());
	}else{
		if(foreground){
			wait_pid(pid);
		}
	}
}

int main(int argc, char* argv[], char* envp[]) {
	init_shell();
	while(1){
		uprintf("\n##ADIX >> ");
		run_shell();

	}
	
}


