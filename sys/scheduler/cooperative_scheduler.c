#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>

void invokeHello();
void invokeWorld();
void invokeDaemon();

/* Test method used to initialize a system that does a co-operative
scheduling.*/
void cooperative_schedule(){
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeHello));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeWorld));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeDaemon));
	printPcbRunQueue();
	schedule(); 
}
