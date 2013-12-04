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
		exit(0);
	}
	
	for(i = 1; i< argc; i++){
		ls(argv[i]);
	}
	exit(0);
}

void ls(char *path){
	int fd;
	fd = opendir(path);
	if(fd == -1){
		printf("ls: cannot open %s",path);
		return;
	}
	struct posix_header_ustar *readdir_return;
	printf("Deepa");
	while((readdir_return = readdir(fd))!=0){
		printf("Deepa2");

		printf("element: %s\n", readdir_return->name);
	}
	int close_return = close(fd);
	printf("close_return: %d",close_return);


}
