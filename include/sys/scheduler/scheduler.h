#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<sys/scheduler/pcb.h>
#include<defs.h>
void switchTo(void*);
uint64_t saveReturnAddress();
void schedule();
void exit();
void addToTaskList(struct pcb_t*);
void printPcbRunQueue();
void cleanupTerminated();
#endif
