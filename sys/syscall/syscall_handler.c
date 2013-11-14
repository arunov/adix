#include<sys/scheduler/scheduler.h>
#include<sys/syscall/syscall.h>
#include<sys/irq.h>
#include<sys/kstdio.h>
#include<defs.h>

void syscallHandler(uint64_t num){
	uint64_t syscall;
	__asm__("movq %%rax,%0;"
		: "=r"(syscall)
		:);
	if(num == SYS_YIELD){
		schedule();
	}
}

void syscallInstall(){
	irq_install_handler(16, (void (*)()) syscallHandler);	
}
