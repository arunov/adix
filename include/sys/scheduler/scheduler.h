#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<sys/scheduler/pcb.h>
#include<defs.h>
void switchTo(void*,uint64_t);
uint64_t saveReturnAddress();
void schedule();
void addToTaskList(struct pcb_t*);
#endif
