#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/kstdio.h>
#include<sys/list.h>
#include<sys/memory/handle_cr2_cr3.h>
LIST_HEAD(pcb_run_queue);
LIST_HEAD(pcb_terminated_queue);
LIST_HEAD(pcb_wait_queue);

/* Get next task that is ready to be run*/
static struct pcb_t* getNextTask(){
	return list_entry(pcb_run_queue.next,struct pcb_t,lister);
}

struct pcb_t* getCurrentTask(){
	return list_entry(pcb_run_queue.prev,struct pcb_t,lister);
}

struct pcb_t* getTerminatedTask(){
	if(list_empty(&pcb_terminated_queue)){
		return NULL;
	}
	return list_entry(pcb_terminated_queue.next,struct pcb_t,lister);
}

void sys_yield(){
	printf("\n#####Yielding to next process######");
	struct pcb_t *nextTask = getNextTask();
	list_del(&nextTask->lister);//remove from head
	list_add_tail(&nextTask->lister,&pcb_run_queue); //add to tail
	set_cr3(*(struct str_cr3*)(&nextTask->cr3_content));
	switchTo(nextTask->stack_base);
}

void sys_exit(int status){ 
	struct pcb_t *current_task = getCurrentTask();
	updateState(current_task, P_TERMINATED);
	list_del(&current_task->lister);//delete from run queue
	//add to terminated queue
	list_add(&current_task->lister,&pcb_terminated_queue);
	sys_yield();//schedule next job
}

void sys_sleep(uint64_t wait_desc){
	struct pcb_t *current_task = getCurrentTask();
	update_wait_descriptor(current_task, wait_desc);
	list_del(&current_task->lister);
	list_add(&current_task->lister, &pcb_wait_queue);
	printf("\n#####SLEEPING PROCESS %d",current_task->pid);
	sys_yield();
}

static void wakeup_proc(struct pcb_t *waiting_task){
	update_wait_descriptor(waiting_task, NOT_WAITING);
	list_del(&waiting_task->lister);//delete from wait queue
	list_add(&waiting_task->lister, &pcb_run_queue);//add to run queue
	printf("\n#####WAKING UP process %d",waiting_task->pid);
	sys_yield();
}

void sys_wakeup(uint64_t wait_desc){
	struct pcb_t *proc;
	list_for_each_entry(proc, &pcb_wait_queue, lister){
		if(proc->wait_desc == wait_desc){
			wakeup_proc(proc);
		}
	}
	printPcbRunQueue();
}

void cleanupTerminated(){
	struct pcb_t *terminated_task = getTerminatedTask();	
	if(terminated_task != NULL){
		list_del(&terminated_task->lister);
		printf("\nContenets of PCB after cleanup..");	
		printPcbRunQueue();
	}
}

void addToTaskList(struct pcb_t *pcb){
	list_add_tail(&pcb->lister,&pcb_run_queue);
}

void printPcbRunQueue(){
	struct pcb_t *the_pcb = NULL;
	printf("\nPCB READY QUEUE: ");
	list_for_each_entry(the_pcb,&pcb_run_queue,lister){
		printPcb(the_pcb);
	}
}