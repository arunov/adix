#include <stdlib.h>
#include <stdio.h>
#include <syscall.h>
#include <defs.h>
#include <files.h>

void ls(char *path);

int main(int argc, char* argv[], char* envp[]) {
	int i;
	if(argc <=1){
		ls(".");
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
