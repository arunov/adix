#include<sys/syscall/syscall.h>
#include<defs.h>

void trap(uint64_t syscall_num){
	__asm__("movq %0,%%rax;"
		"int $48;"
		:
		:"r"(syscall_num)
		:
		);
}

void yield(){
	trap(SYS_YIELD);	
}
