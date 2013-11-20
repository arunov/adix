#include <syscall.h>
#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/memory/handle_cr2_cr3.h>
int counter_hello=0;
int sleep_count = 0;
/* A Helo method that yields to next process in run queue*/
void invokeHello(){
	while(1){
		if(counter_hello % 3000000 == 0){
			counter_hello = 0;
			uprintf("\nHello...");
			sleep(1);
		}
		counter_hello++;

		//printf(" CR3 is:%p",get_cr3());
	}
	printf("\n......................");
	printf("Exiting hello..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	sys_exit(0);
}
