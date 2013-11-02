#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
void invokeHello();
void invokeWorld();

void cooperative_schedule(){
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeHello));
	addToTaskList(createTask(KTHREAD,(uint64_t)&invokeWorld));
	schedule(); 
}
