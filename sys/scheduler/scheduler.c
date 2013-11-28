#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/kstdio.h>
#include<sys/list.h>
#include<sys/memory/handle_cr.h>
LIST_HEAD(pcb_run_queue);
LIST_HEAD(pcb_terminated_queue);
LIST_HEAD(pcb_wait_queue);
LIST_HEAD(wait_timer_queue);

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
//	printf("\n#####Yielding to next process######");
	struct pcb_t *nextTask = getNextTask();
	list_del(&nextTask->lister);//remove from head
	list_add_tail(&nextTask->lister,&pcb_run_queue); //add to tail
	if(nextTask->cr3_content != NULL){
		set_cr3(*(struct str_cr3*)(&nextTask->cr3_content));
	}
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
	#ifdef DEBUG
	printf("\n#####SLEEPING PROCESS %d",current_task->pid);
	#endif
	sys_yield();
}

static void wakeup_proc(struct pcb_t *waiting_task){
	update_wait_descriptor(waiting_task, NOT_WAITING);
	list_del(&waiting_task->lister);//delete from wait queue
	list_add(&waiting_task->lister, &pcb_run_queue);//add to run queue
	#ifdef DEBUG
	printf("\n#####WAKING UP process %d",waiting_task->pid);
	#endif
}


void _sys_wakeup(struct list_head *wait_queue, uint64_t wait_desc){
	struct pcb_t *proc;
	struct pcb_t *temp[MAX_WAIT_PROC]; //TODO: Poses an upper bound?
	int i=0;
	list_for_each_entry(proc, wait_queue, lister){
		if(proc->wait_desc == wait_desc){
			temp[i++] = proc;
		}
	}
	while(i != 0){
		wakeup_proc(temp[--i]);
	}
//	sys_yield();
}

void sys_wakeup_timer(uint64_t pid){
	_sys_wakeup(&wait_timer_queue, pid);
}

void sys_wakeup(uint64_t wait_desc){
	_sys_wakeup(&pcb_wait_queue, wait_desc);
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
	#ifdef DEBUG
	printf("\nPCB READY QUEUE: ");
	#endif
	list_for_each_entry(the_pcb,&pcb_run_queue,lister){
		printPcb(the_pcb);
	}
}
