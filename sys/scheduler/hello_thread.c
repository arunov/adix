#include<sys/kstdio.h>
#include<sys/scheduler/scheduler.h>

int counter2=4;
void invokeHello(){
	while(counter2-->0){	
		printf("Hello...\n");
		schedule();
		printf("World yields..");
	}
	printf("\n......................");
	printf("\nexiting hello..");
//	exitSchedule();
}
