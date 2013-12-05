#include <stdio.h>
#include <string.h>
#include <syscall.h>
#define BUF_SIZE 1024
#define NUM_ARGS 10
#define DELIM " "
#define PATH_DELIM ":"
#define BG_SYMBOL "bg"

char *self = "bin/adsh";
char path[512] = "bin/";
char child_argv_path[512];

int setenv(int argc, char* argv[], char* envp[]);

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

char* resolve_path(char *command, char* path_token){
	
	char *ptr;

	memcpy(child_argv_path, path_token, (strlen(path_token)+ 1) );
	ptr = child_argv_path +strlen(path_token);
	memcpy(ptr, command, (strlen(command)+1));
	return child_argv_path;
	
}

// supports "setenv path value"
int setenv(int argc, char* argv[], char* envp[]){
	if(argc!=3){
		printf("setenv failed");
		return -1;
	}
	if((str_equal(argv[1],"path"))==1){
		memcpy(path,argv[2],strlen(argv[2])+1); 
		return 0;
	}
	else{
		printf("setenv failed");
		return -1;
	}	
}

//used with "export" or "export ..."
void display_env_var(){
	printf("PATH: %s\n",path);
}

char** get_env(){
	return NULL;//TODO
}



void exec_command(int ecmd_argc, char *ecmd_argv[], char *envp[]){
	int foreground = is_foreground(ecmd_argc, ecmd_argv);
	int64_t pid = 0;
	//printf("Is foreground: %d\n",foreground);
	pid = fork();
    if(pid == -1) {
        printf("fork failed\n");
	} else if(pid == 0){
	//	printf("Execing %s\n",ecmd_argv[0]);
		char *buffer = path, *path_token;
		int i = 0;
		int len = 0;
		char *command = ecmd_argv[0];
		execvpe(ecmd_argv[0], ecmd_argv, envp);

		for(i = 0; *buffer!= NULL; i++){
			path_token = strtok(buffer,PATH_DELIM);	
			ecmd_argv[0] = resolve_path(command, path_token);
			execvpe(ecmd_argv[0], ecmd_argv, envp);
			len = strlen(path_token);
			buffer += len+1;
		}
		
		printf("\nCommand \"%s\" failed\n",command);
		exit(0);//If execvpe returned => it failed
	
	
	} else{
		if(foreground){
			wait_pid(pid);
		}
	}
}

/* Run the shell that reads inputs and executes them*/
void run_shell(){
	int child_argc;
	int count = 1;
	while(1){
		printf("\n##ADIX[%d]>> ",count++);
		char *buffer = malloc(BUF_SIZE);//each process should get its own buffer
		char **child_argv = (char**) malloc(NUM_ARGS * sizeof(char*));
		int count = read(STDIN, buffer, BUF_SIZE);
		//write(STDOUT, buffer, count);
		buffer[count] = '\0';
		child_argc = parse_shell_command_args((char*)buffer, child_argv);
		if(str_equal(child_argv[0],"setenv")==1){
			setenv(child_argc, child_argv, get_env());	
		}
		else if(str_equal(child_argv[0],"export")==1){
			display_env_var();
			
		}
		
		else{
			exec_command(child_argc, child_argv, get_env());
		}
	}
}

/* Given a string representing a command, parses the command and executes it*/
void process_command_str(char *command_str, char *env[]){
	char **cmd_argv = (char**) malloc(NUM_ARGS * sizeof(char*));
	int cmd_argc = parse_shell_command_args(command_str, cmd_argv);
	exec_command(cmd_argc, cmd_argv, env);
}

/* Read next line from the file_content passed, at an offset of chars_read */
char* get_next_line(char* file_content, int file_size, int *chars_read){
	file_content = file_content + *chars_read;
 	char *line_beg = file_content;
	while(*file_content != '\n' 
			&& *file_content != '\0' 
			&&*chars_read != file_size){
		*chars_read += 1;
		file_content++;
	}
	*chars_read += 1;//account for \n
	*file_content='\0';//Replace \n with \0
	return line_beg;
}

/* Given a shebang line, returns the interpreter for the script */
char* get_interpreter(char *she_bang_line){
	//skip_leading_white_spaces
	while(*she_bang_line == ' ' || *she_bang_line == '\t'){she_bang_line++;}
	if(she_bang_line[0] == '#' && she_bang_line[1] == '!'){
		//remove leading spaces
		return &she_bang_line[2];
	}	
	return NULL;
}

/* Execute the shell script passed as argv[2] */
void exec_shell_script(int argc, char *argv[], char *envp[]){
	char *file_name = argv[2];
	char *next_line;
	int fd = open(file_name);
	if(fd == -1){
		printf("File not found %s",file_name);
		return;
	}
	char *file_content = malloc(BUF_SIZE);
	//TODO: Run one after the other till end
	int file_size = read(fd,file_content,BUF_SIZE);
	int chars_read = 0;
	//TODO: Skip empty lines
	char *she_bang_line = get_next_line(file_content, file_size, &chars_read);
	char *interpreter = get_interpreter(she_bang_line);

	if(interpreter == NULL){
		printf("No interpreter specified");
		/* No interpreter was specified, make it self */
		interpreter = self;
		/* Undo last read */
		chars_read -= (strlen(she_bang_line)+1); 
	}
	
	/* Run the right interpreter */
	if(str_equal(interpreter, self) == 0){
		printf("Running interpreter %s\n",interpreter);
		/* Interpreter is different, exec that interpreter */
		argv[0] = interpreter;
		/* Pass same args */
		exec_command(argc, argv, envp);
		/* Return parent */
		return;
	}

	/* Interpreter is self, continue executing commands */
	while(chars_read != file_size){
		next_line = get_next_line(file_content, file_size, &chars_read);
		if(strlen(next_line) == 0){
			/* Skip empty lines */
			continue;
		}
		process_command_str(next_line, envp);
	}	
}

int main(int argc, char* argv[], char* envp[]) {
	if(argc>1){
		if(str_equal(argv[1],"-f")){
			exec_shell_script(argc, argv, envp);
		}else{
			exec_command(argc-1, &argv[1], envp);
		}
	} else{
		init_shell();
		run_shell();
	}
	exit(0);
}
