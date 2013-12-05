#include <stdio.h>
#include <string.h>
#include <syscall.h>
#define BUF_SIZE 1024
#define NUM_ARGS 10
#define DELIM " "
#define PATH_DELIM ":"
#define BG_SYMBOL "&"
#define NUM_ENV_VARS 100

char *self = "bin/adsh";
char path[512] = "bin/";
char child_argv_path[512];
char **env;
char **env_names;


void exec_shell_script(int argc, char *argv[], char *envp[]);

int parse_shell_command_args(char *buffer, char *child_argv[]){

	int len = 0;
	int i = 0;
	int buflen = strlen(buffer)-1;
	for (i=0; *buffer != '\0'; i++){
			/* This strtok does not rememeber where it left last time.
			So remember here where it stopped last time */
			child_argv[i] = strtok(buffer,DELIM);
			len = strlen(child_argv[i]);
			buffer += len+1;
			buflen--;
	}
	child_argv[i] = NULL;
	return i;
}
int is_foreground(int argc, char *child_argv[]){
	return str_equal(child_argv[argc-1],BG_SYMBOL) ? 0 : 1;
}

void init_shell(){
	env = (char**)malloc(sizeof(char*) * NUM_ENV_VARS);
	memset(env, 0,sizeof(char*) * NUM_ENV_VARS); 
	env_names = (char**)malloc(sizeof(char*) * NUM_ENV_VARS);
	memset(env_names, 0,sizeof(char*) * NUM_ENV_VARS); 
	char *argv[] = {"bin/adsh", "-f", ".adshrc"};
	exec_shell_script(3, argv, env); 
}

void init_shell_env(char *envp[]){
	env = (char**)malloc(sizeof(char*) * NUM_ENV_VARS);
	memset(env,0,sizeof(char*) * NUM_ENV_VARS); 
	env_names = (char**)malloc(sizeof(char*) * NUM_ENV_VARS);
	memset(env_names, 0,sizeof(char*) * NUM_ENV_VARS); 
	int i;
	env = envp;
	for(i=0; envp[i] != NULL; i++){
		char *env_name = malloc(strlen(envp[i])+1);
		env[i] = envp[i];
		memcpy(env_name,envp[i], strlen(envp[i])+1);
		env_names[i] = strtok(env_name, "=");	
	}
}

char* resolve_path(char *command, char* path_token){
	char *ptr;
	memcpy(child_argv_path, path_token, (strlen(path_token)+ 1) );
	ptr = child_argv_path +strlen(path_token);
	memcpy(ptr, command, (strlen(command)+1));
	return child_argv_path;
	
}

/* Scan through the list of env variables and returns location where 
 * where the env for the key passed was stored. If the key does not
 * exist, returns the next available index */
int get_env_location(char *key){
	int i=0;
	while(env_names[i] != NULL){
		if(str_equal(env_names[i], key)){
			return i;
		}
		i++;
	}
	env[i+1] = NULL;  //TODO: Why do we need this?
	return i;
}

/* Creates an entry in the global environment table if key does not exist
 * already. Replaces an existing entry */
int update_global_env(char *key, char *value){
	if(key == NULL || value == NULL){
		return -1;
	}
	int i = get_env_location(key);
	char *env_str = malloc(strlen(key) + strlen(value) + 2);
	int key_len = strlen(key);
	int value_len = strlen(value);
	/* Create an environment string */
	memcpy(env_str, key, key_len);
	memcpy(env_str + key_len, "=", 1);
	memcpy(env_str + key_len + 1, value, value_len + 1);
	env_names[i] = key;
	env[i] = env_str;
	return 0;
}


int setenv(int argc, char* argv[]){
	if(argc!=3){
		printf("setenv failed");
		return -1;
	}
	if((str_equal(argv[1],"path"))==1){
		memcpy(path,argv[2],strlen(argv[2])+1); 
	}
	return update_global_env(argv[1],argv[2]);
}

/* Execute the binary corresonding to the command */
void exec_command(int ecmd_argc, char *ecmd_argv[], char *envp[]){
	int foreground = is_foreground(ecmd_argc, ecmd_argv);
	int64_t pid = 0;
	pid = fork();
    if(pid == -1) {
        printf("fork failed\n");
	} else if(pid == 0){
		char *buffer = path, *path_token;
		int i = 0;
		int len = 0;
		char *command = ecmd_argv[0];
		execvpe(ecmd_argv[0], ecmd_argv, envp);

		for(i = 0; *buffer != NULL; i++){
			path_token = strtok(buffer,PATH_DELIM);	
			ecmd_argv[0] = resolve_path(command, path_token);
			execvpe(ecmd_argv[0], ecmd_argv, envp);
			len = strlen(path_token);
			buffer += len+1;
		}
		printf("\nCommand %s failed\n",command);
		exit(0);//If execvpe returned => it failed
	
	
	} else{
		if(foreground){
			wait_pid(pid);
		}
	}
}

void get_env(char *key){
	int i = 0;
	if(key == NULL){
		return;	
	}
	while(env_names[i] != NULL){
		if(str_equal(env_names[i],key)){
			printf(env[i]);
			printf("\n");
			return;
		}
		i++;
	}
	printf("Environment variable %s not defined", key);
}

/* Verify the directory and change current working directory */
int change_dir(int argc, char *argv[]){
	if(argc != 2){
		printf("cd: Path not specified\n");
		return -1;
	}
	char *dir = argv[1];
	int fd = opendir(dir);
	if(fd == -1){
		printf("cd : Could not open directory %s\n",dir);
		return -1;
	}
	closedir(fd);
	return update_global_env("pwd",dir);
}

void get_all_env(){
	for(int i=0; env[i] != NULL; i++){
		printf("%s \n",env[i]);
	}
}

static int process_shell_jobs(int argc, char *argv[]){
	char *arg = argv[0];
	if(str_equal(arg,"setenv")){
		setenv(argc, argv);	
	} else if (str_equal(arg, "getenv")){
		get_env(argv[1] );
	} else if(str_equal(arg, "env")){
		get_all_env();
	}else if(str_equal(arg, "cd")){
		change_dir(argc, argv);
	} else{
		return 0;
	}
	return 1;
}

void process_command(int cmd_argc, char *cmd_argv[]){
	/* Needs no execing. Just updates shell status */
	int shell_job = process_shell_jobs(cmd_argc, cmd_argv);
	if(shell_job == 0){
		/* Not a shell job, needs execing */
		exec_command(cmd_argc, cmd_argv, env);
	}

}

/* Given a string representing a command, parses the command and executes it*/
void process_command_str(char *command_str, char *env[]){
	char **cmd_argv = (char**) malloc(NUM_ARGS * sizeof(char*));
	int cmd_argc = parse_shell_command_args(command_str, cmd_argv);
	process_command(cmd_argc, cmd_argv);
}

/* Run the shell that reads inputs and executes them*/
static void run_shell(){
	while(1){
		printf("\n##ADIX>> ");
		char *buffer = malloc(BUF_SIZE);//each process should get its own buffer
		int count = read(STDIN, buffer, BUF_SIZE);
		buffer[count] = '\0';
		if(strlen(buffer) == 0){
			/* No input received, continue */	
			continue;
		}
		process_command_str(buffer, NULL);
	}
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

/* Reads the size of the file by recursive read till EOF */
uint64_t get_file_size(char *file_name){
	int file_size = 0, count;
	int fd = open(file_name);
	if(fd == -1){
		printf("File not found %s",file_name);
		return 0;
	}
	char *file_content = malloc(10);
	do{
		count =  read(fd,file_content,10);
		file_size += count;
	}while(count != 0);

	close(fd);
	free(file_content);
	return file_size;
}

/* Execute the shell script passed as argv[2] */
void exec_shell_script(int argc, char *argv[], char *envp[]){
	char *file_name = argv[2];
	char *next_line;
	int file_size = get_file_size(file_name);
	if(file_size == 0) 
		return; 
	/* Read complete contents from file */
	int fd = open(file_name);
	char *file_content = malloc(file_size+1);
	read(fd, file_content, file_size);
	file_content[file_size] = '\0';
	close(fd);
	/* Start to process the files */
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
		process_command(argc,argv);
		/* Return parent */
		return;
	}

	/* Interpreter is self, continue executing commands */
	while(chars_read != file_size){
		next_line = get_next_line(file_content, file_size, &chars_read);
		/* strtok does something evil, so being safe here!*/
		char *temp = malloc(strlen(next_line)+2);
		memcpy(temp, next_line, strlen(next_line)+1);
		temp[strlen(next_line) + 2] = '\0';
		if(strlen(next_line) == 0){
			/* Skip empty lines */
			continue;
		}
		printf("::Cmd:\"%s\"\n",temp);
		process_command_str(temp, envp);
	}	
}

void process_exec_jobs(int argc, char *argv[], char *envp[]){
	if(str_equal(argv[1],"-f")){
		exec_shell_script(argc, argv, envp);
	}else{
		exec_command(argc-1, &argv[1], envp);
	}
}

int main(int argc, char* argv[], char* envp[]) {
	if(argc == 1){
		clrscr();
		init_shell();
		run_shell();
	}else {
		init_shell_env(envp);
		process_exec_jobs(argc, argv, envp);
	}
	exit(0);
}
