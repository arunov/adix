#include<sys/kstdio.h>
#include<sys/scheduler/scheduler.h>
#include<sys/syscall/syscall.h>
#include<sys/memory/handle_cr2_cr3.h>
#include<sys/memory/pg_tbl_manager.h>

static int counter_world = 4;

/* A World method that yields to next process in run queue*/
void invokeWorld(){
	while(counter_world--){
		get_new_pg_tbl_page();

		printf("World..");
//		if(counter_world == 3 ){
//			printf(" CR3 is:%p",get_cr3());
//		}
		yield();
		printf("\nContinuing in world..");
	}
	printf("\n.....................");
	printf("Exiting world..");
	//Exit this process. Sets up cleaning up of PCBs as of now.
	exit();
}
