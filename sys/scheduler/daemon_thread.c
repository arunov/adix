#include<sys/scheduler/scheduler.h>

/* Idle process for the system. This process eats CPU
cycles. However it is required to ensure that atleast one
kernel thread is running at any point of time.*/
void invokeDaemon(){
	while(1){
		schedule();
	}
}
