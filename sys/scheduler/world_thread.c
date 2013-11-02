#include<sys/kstdio.h>
#include<sys/scheduler/scheduler.h>
int counter1 = 4;
void invokeWorld(){
	while(counter1-->0){
		printf("World..\n");
		schedule();
		printf("Hello yields..");
	}
	printf("\n.....................");
	printf("\nExiting world..");
}


