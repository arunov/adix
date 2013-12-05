#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <defs.h>

#define YIELD 0
#define PRINTF 1
#define EXIT 2
#define SLEEP 10
#define CLRSCR 12
/* File system system calls */
#define OPEN 3
#define READ 4
#define LSEEK 5
#define CLOSE 6
#define OPENDIR 7
#define READDIR 8
#define CLOSEDIR 9
#define WRITE 11

#define MALLOC 15
#define EXEC 16
#define GETPID 17
#define FORK 18
#define WAITPID 19
#define WAIT 20
#define PROCESS_SNAPSHOT 21
#define FREE 22
#define GETRLIMIT 23
#define SETRLIMIT 24
#define MMAP 25
#define MUNMAP 26

#define SYSCALL_PROTO(num) static inline uint64_t __syscall##num

void yield();
void exit(int status);
int sleep(int64_t sleep_interval);
int64_t wait_pid(uint64_t pid);
int64_t wait();
uint64_t uprintf(const char *format_string);

/* FIle system operations */
int open(const char* filename);
int64_t read(int fd, void *buf, uint64_t count);
int64_t write(int fd, void *buf, uint64_t count);

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
int lseek(int fd, off64_t offset, int whence);

int close(int fd);
int opendir(const char *pathname);
struct posix_header_ustar* readdir(int fd);
int closedir(int fd);
void clrscr();
int64_t execvpe(char *path, char *argv[], char *envp[]);
int64_t fork();
/*Memory operations*/
void* malloc(uint64_t size);
void free(void *ptr);

#define MMAP_RW       0x2ULL
#define MAP_ANONYMOUS 0x00000001U
void* ummap(void *addr, uint64_t length, uint64_t prot, int flags);
void umunmap(void *addr);

/* Process */
uint64_t get_pid();

enum ps_state {
    RUNNING,
    WAITING,
    WAITING_TIMER
};

struct ps_t {
    uint64_t pid;
    char *name;
    enum ps_state state;
    struct ps_t *next;
};

/**
 * Get list of live processes
 * @param list *list gets populated with process list
 */
void process_snapshot(struct ps_t **list);
void free_ps_list(struct ps_t **list);

/* User space system call stub for all system calls with 'zero' arguments. */
SYSCALL_PROTO(0)(uint64_t syscall_num) {
	uint64_t ret;
	__asm volatile(	"movq %1,%%rax;"
		"int $48;"
		"movq %%rax,%0;"
		:"=r"(ret)
		:"r"(syscall_num)
		:"rax"
		);
	return ret;
}

/* User space system call stub for all system calls with 'one' argument. */
SYSCALL_PROTO(1)(uint64_t syscall_num, uint64_t a1) {
	uint64_t ret;
	/* There is a problem with backing up of registers - rax cannot br popped
	in the end since its used for returning the same return value. I suppose
	this is some kind of a compiler optimization! */
	__asm volatile("movq %1,%%rax;"
		"movq %2, %%rdi;"
		"int $48;"
		"movq %%rax,%0;"
		:"=r"(ret)
		:"r"(syscall_num),"r"(a1)
		:"rax","rdi"//Clobberred registers
		);
	return ret;
}

SYSCALL_PROTO(2)(uint64_t syscall_num, uint64_t a1, uint64_t a2) {
	uint64_t ret;
	__asm volatile(
		"movq %1,%%rax;"
		"movq %2, %%rdi;"
		"movq %3, %%rsi;"
		"int $48;"
		"movq %%rax,%0;"
		:"=r"(ret)
		:"r"(syscall_num),"r"(a1),"r"(a2)
		:"rax","rdi","rsi" //Clobberred registers
		);
	return ret;
}

SYSCALL_PROTO(3)(uint64_t syscall_num, uint64_t a1, uint64_t a2, uint64_t a3) {
	uint64_t ret;
	__asm volatile(
		"movq %1,%%rax;"
		"movq %2, %%rdi;"
		"movq %3, %%rsi;"
		"movq %4, %%rdx;"
		"int $48;"
		"movq %%rax,%0;"
		:"=r"(ret)
		:"r"(syscall_num),"r"(a1),"r"(a2),"r"(a3)
		:"rax","rdi","rsi","rdx" //Clobberred registers
		);
	return ret;
}

SYSCALL_PROTO(4)(uint64_t syscall_num, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
	uint64_t ret;
	__asm volatile(
		"movq %1,%%rax;"
		"movq %2, %%rdi;"
		"movq %3, %%rsi;"
		"movq %4, %%rdx;"
		"movq %5, %%rcx;"
		"int $48;"
		"movq %%rax,%0;"
		:"=r"(ret)
		:"r"(syscall_num),"r"(a1),"r"(a2),"r"(a3),"r"(a4)
		:"rax","rdi","rsi","rdx","rcx" //Clobberred registers
		);
	return ret;
}

#endif
