#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/parser/exec.h>

void invokeHello();
void invokeWorld();
void invokeSouj();
void invokeDaemon();
void invokeEcho();

/* Test method used to initialize a system that does a co-operative
scheduling.*/
void cooperative_schedule(void *kernmem, void *physfree){
	addToTaskList(createTask(UPROC,0x0,"bin/adsh",kernmem,  physfree));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeDaemon,0, kernmem, physfree));
	printPcbRunQueue();
	sys_yield(); 
}
