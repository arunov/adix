#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/memory/handle_cr.h>

int counter_world = 0;

/* A World method that yields to next process in run queue*/
void invokeWorld(){
	while(1){
		if(counter_world % 3000000 == 0){
			counter_world = 0;
			printf("\nWorld..");
		}
		counter_world++;
	}
	printf("\n.....................");
	printf("Exiting world..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	sys_exit(0);
}
