#include<defs.h>
#include<syscall.h>
#include<sys/parser/tarfs.h>
uint64_t ret;
void yield(){
	__syscall0(YIELD);
}

void exit(int status){
	__syscall1(EXIT,status);
}

int open(const char* pathname){
	int ret = __syscall1(OPEN, (uint64_t)pathname);
//	printf("\nFd returned to syscall %d",ret);
	return ret;
}

int64_t read(int fd, void *buf, uint64_t count){
	uint64_t ret = __syscall3(READ, fd, (uint64_t)buf, count);
	return ret;
}

int64_t write(int fd, void *buf, uint64_t count){
	uint64_t ret = __syscall3(WRITE, fd, (uint64_t)buf, count);
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


int sleep(int64_t sleep_interval){
 	return __syscall1(SLEEP, sleep_interval);
}

uint64_t uprintf(const char *format_string){
	return __syscall1(PRINTF,(uint64_t)format_string);
}

void clrscr(){
	 __syscall0(CLRSCR);
}

void* malloc(uint64_t size){
	return	(void*)__syscall1(MALLOC, size);
}

int64_t execvpe(char *path, char *argv[], char *envp[]){
	return (int64_t)__syscall3(EXEC, (uint64_t)path, (uint64_t)argv, (uint64_t)envp);	
}

uint64_t get_pid(){
	return __syscall0(GETPID);
}

int64_t fork() {
    return (uint64_t)__syscall0(FORK);
}

int64_t wait_pid(uint64_t pid){
	return __syscall1(WAITPID, pid);
}

int64_t wait(){
	/* Will wait for a child to wakeup itself */
	uint64_t pid = get_pid();
	return __syscall1(WAITPID, pid);
}

void process_list() {
    __syscall0(PROCESS_LIST);
}

