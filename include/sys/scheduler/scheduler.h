#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<sys/scheduler/pcb.h>
#include<defs.h>

#define NOT_WAITING 0x00
/* Method used to switch to another kernel thread*/
void switchTo(void*);
/*Schedule a process/task*/
void sys_yield();
/* Exits from a process. Moves the task from run queue to terminated queue */
void sys_exit(int status);
/* Put current process onto sleep will somebody wakes him up*/
void sys_sleep(uint64_t wait_desc);
/* Wake up all the processor that are waiing for wait_desc*/
void sys_wakeup(uint64_t wait_desc);
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
