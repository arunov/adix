#include<sys/kstdio.h>
#include<sys/scheduler/scheduler.h>
#include<sys/syscall/syscall.h>
static int counter_hello=4;

/* A Helo method that yields to next process in run queue*/
void invokeHello(){
	while(counter_hello--){	
		printf("Hello...");
		yield();
		printf("\nContinuing in hello...");
	}
	printf("\n......................");
	printf("Exiting hello..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	exit();
}
