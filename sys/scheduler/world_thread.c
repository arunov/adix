#include<sys/kstdio.h>
#include<sys/scheduler/scheduler.h>
#include<sys/syscall/syscall.h>

static int counter_world = 4;

/* A World method that yields to next process in run queue*/
void invokeWorld(){
	while(counter_world--){
		printf("World..");
		yield();
		printf("\nContinuing in world..");
	}
	printf("\n.....................");
	printf("Exiting world..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	exit();
}
