#include<defs.h>
#include<syscall.h>
#include<sys/parser/tarfs.h>
struct posix_header_ustar ret = {};
void yield(){
	__syscall0(YIELD);
}

void exit(int status){
	__syscall1(EXIT,status);
}

<<<<<<< HEAD
int open(const char* pathname){
	int ret = __syscall1(OPEN, (uint64_t)pathname);
	return ret;
}

int64_t read(int fd, void *buf, uint64_t count){
	uint64_t ret = __syscall3(READ, fd, (uint64_t)buf, count);
	return ret;
}

int lseek(int fd, off64_t offset, int whence){
	int ret = __syscall3(LSEEK, fd, offset, whence);
	return ret;
}

int close(int fd){
	int ret =__syscall1(CLOSE, fd);
	return ret;
}

int opendir(const char *pathname){
	int ret = __syscall1(OPENDIR, (uint64_t)pathname);
	return ret;
}

struct posix_header_ustar* readdir(int fd){
	//TODO malloc
	return (struct posix_header_ustar*)__syscall2(READDIR, fd, (uint64_t)&ret);
}

int closedir(int fd){
	int ret = __syscall1(CLOSEDIR, fd);
	return ret;
}


=======
void sleep(int sleep_interval){ //TODO
	__syscall1(SLEEP, sleep_interval);
}
>>>>>>> d2f11f461ca50a6e199f87394b4d5ebcdd09e2e4
