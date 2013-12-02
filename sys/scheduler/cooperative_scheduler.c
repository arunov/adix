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
	//replace createTask with exec
//	addToTaskList(createTask(UPROC,0x0,"bin/hello",kernmem,  physfree));
//	addToTaskList(createTask(UPROC,0x0,"bin/world",kernmem,  physfree));
	addToTaskList(createTask(UPROC,0x0,"bin/adix_shell",kernmem,  physfree));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeDaemon,0, kernmem, physfree));
//	addToTaskList(createTask(UPROC,0x0,"bin/world",kernmem,  physfree));
//	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeEcho,0, kernmem, physfree));
//	addToTaskList(createTask(UPROC,0x0,(uint64_t)&invokeHello,kernmem,  physfree));
//	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeWorld, kernmem, physfree));
//	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeSouj, kernmem, physfree));
	printPcbRunQueue();
	sys_yield(); 
}
