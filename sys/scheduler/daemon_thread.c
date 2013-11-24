#include<sys/scheduler/scheduler.h>
#include <sys/kstdio.h>
#include <syscall.h>
/* Idle process for the system. This process eats CPU
cycles. However it is required to ensure that atleast one
kernel thread is running at any point of time.*/
int counter  = 0;
void invokeDaemon(){
	while(1){	
		sys_yield();
		if(counter++ == 3){
//			sys_wakeup(1);
		}
	};
}
