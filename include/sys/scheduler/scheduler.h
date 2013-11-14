#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<sys/scheduler/pcb.h>
#include<defs.h>
/* Method used to switch to another kernel thread*/
void switchTo(void*);
/*Schedule a process/task*/
void schedule();
/* Exits from a process. Moves the task from run queue to terminated queue */
void exit();
/* Add a task to the pcb run queue*/
void addToTaskList(struct pcb_t*);
/* Print entries on the run queue*/
void printPcbRunQueue();
/* Clear PCB and related information when a process is terminated. 
   This methods is called after the context is switched to a new process.*/
void cleanupTerminated();
/* Get current task that is being run by this CPU */
struct pcb_t* getCurrentTask();

#endif
