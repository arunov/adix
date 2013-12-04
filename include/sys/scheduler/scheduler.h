#ifndef SCHEDULER_H
#define SCHEDULER_H
#include<sys/scheduler/pcb.h>
#include<defs.h>

#define NOT_WAITING 0x00
#define MAX_WAIT_PROC 20
/* Method used to switch to another kernel thread*/
void switchTo(void*);
/*Schedule a process/task*/
void sys_yield();
/* Exits from a process. Moves the task from run queue to terminated queue */
void sys_exit(int status);
/* Put current process onto sleep will somebody wakes him up*/
int64_t sys_sleep(uint64_t wait_desc);
/* Wake up all the processor that are waiing for wait_desc*/
int sys_wakeup(uint64_t wait_desc);
/* Add a task to the pcb run queue*/
void addToTaskList(struct pcb_t*);
/* Print entries on the run queue*/
void printPcbRunQueue();
/* Clear PCB and related information when a process is terminated. 
   This methods is called after the context is switched to a new process.*/
void cleanupTerminated();
/* Get current task that is being run by this CPU */
struct pcb_t* getCurrentTask();
/* Add task that is under preparation */
void updatePrepTask(struct pcb_t *pcb);
/* Sleep for sleep_interval duration */
int sys_sleep_timer(int64_t sleep_interval);
/* Update sleep_time_remaining and wakeup any process that has elapsed its 
 * timer interval */
void sys_wakeup_timer();
/* Free contents of this pcb - called from cleanup terminated!*/
void deep_free_task(struct pcb_t *pcb);
#endif
