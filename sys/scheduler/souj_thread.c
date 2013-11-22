#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/memory/handle_cr2_cr3.h>

int counter_souj = 0;

/* A World method that yields to next process in run queue*/
void invokeSouj(){
	while(1){
		if(counter_souj % 3000000 == 0){
			counter_souj = 0;
			printf("\nSouj..");
		}
		counter_souj++;
	}
	printf("\n.....................");
	printf("Exiting souj..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	sys_exit(0);
}
