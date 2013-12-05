#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>
#include <files.h>
#include <env.h>

#define PWD_NAME "pwd"

void ls(char *path);

int main(int argc, char* argv[], char* envp[]) {
	int i;
	if(argc <=1){
		char *pwd= get_env(PWD_NAME, envp);
		if(pwd == NULL){
			printf("No environemnt variable %s\n", PWD_NAME);
			return -1;
		}
		printf("Value returned is %s\n",pwd);
		/* ls on present working directory */
		ls(pwd);
		return 0;
	}
	
	for(i = 1; i< argc; i++){
		ls(argv[i]);
	}
	return 0;
}

void ls(char *path){
	int fd;
	
	fd = opendir(path);
	if(fd == -1){
		printf("ls: cannot open %s",path);
		return;
	}
	printf("%s: \n",path);
	struct posix_header_ustar *readdir_return;
	while((readdir_return = readdir(fd))!=0){
		printf("%s\n", readdir_return->name);
	}

	close(fd);
}
