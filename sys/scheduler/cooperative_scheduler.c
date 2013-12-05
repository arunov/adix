#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/parser/exec.h>

void invokeDaemon();

/* Method used to incubate starting processes into the system */
void cooperative_schedule(void *kernmem, void *physfree){
	addToTaskList(createTask(UPROC,0x0,"bin/adsh",kernmem,  physfree));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeDaemon,0, kernmem, physfree));
	printPcbRunQueue();
	sys_yield(); 
}
