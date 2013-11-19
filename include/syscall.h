#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <defs.h>
#define YIELD 0
#define PRINTF 1
#define EXIT 2
#define SLEEP 3

#define SYSCALL_PROTO(num) static inline uint64_t __syscall##num

/* User space system call stub for all system calls with 'zero' arguments. */
SYSCALL_PROTO(0)(uint64_t syscall_num) {
	__asm("pushq %%rax;"
		"movq %0,%%rax;"
		"int $48;"
		"popq %%rax;"
		"retq;"
		:
		:"r"(syscall_num)
		:"rax"
		);
	return 0;
}

/* User space system call stub for all system calls with 'one' argument. */
SYSCALL_PROTO(1)(uint64_t syscall_num, uint64_t a1) {
	__asm("pushq %%rax;"
		"pushq %%rdi;"
		"movq %0,%%rax;"
		"movq %1, %%rdi;"
		"int $48;"
		"popq %%rdi;"
		"popq %%rax;"
		"retq;"
		:
		:"r"(syscall_num),"r"(a1)
		:"rax","rdi" //Clobberred registers
		);
	return 0;
}

SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
	return 0;
}

SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
	return 0;
}

SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
	return 0;
}

void exit(int status);
void yield();
void sleep(int sleep_interval);
#endif
