#include <stdio.h>
#include <syscall.h>
#include <defs.h>


int main(int argc, char* argv[], char* envp[]) {
	int fd = open("aladdin.txt");
	printf("open fd: %d\n",fd);

	char *buffer;
	buffer = (char *)malloc(21);
	int read_count = read(fd, buffer, 20);
	printf("read_count: %d\n", read_count);
	buffer[read_count] = '\0';	
	printf("buffer: %s\n",buffer);

	read_count = read(fd, buffer, 20);
	printf("read_count: %d\n", read_count);
	printf("buffer: %s\n",buffer);

	int close_ret = close(fd);
	printf("close_ret %d\n",close_ret);

	int fd2 = 10;
	close_ret = close(fd2);
	printf("close_ret %d\n",close_ret);
	
	fd2 = open("aladdin.txt");
	printf("open fd2: %d\n",fd2);

	close_ret = close(fd2);
	printf("close_ret %d\n",close_ret);
	
	char *ptr = 0;
	printf("%s",ptr);


}
