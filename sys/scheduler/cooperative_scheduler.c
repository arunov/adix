#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/syscall/syscall.h>

void invokeHello();
void invokeWorld();
void invokeDaemon();

/* Test method used to initialize a system that does a co-operative
scheduling.*/
void cooperative_schedule(void *kernmem, void *physfree){
	//replace createTask with exec
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeHello,kernmem,  physfree));
	addToTaskList(createTask(UPROC,(uint64_t)&invokeWorld, kernmem, physfree));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeDaemon, kernmem, physfree));
	printPcbRunQueue();
	yield(); 
}
