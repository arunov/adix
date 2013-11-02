#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/kstdio.h>
static int counter;
struct pcb_t *pcb_aa[2];

struct pcb_t* getNextTask(){
	return pcb_aa[counter++%2];
}

void schedule(){
	printf("scheduling.. ");
	uint64_t saved_rip = saveReturnAddress();
	struct pcb_t *nextTask = getNextTask();
	switchTo(nextTask->stack_base,saved_rip);
}

void addToTaskList(struct pcb_t *pcb){
	pcb_aa[counter++%2] = pcb;
}

