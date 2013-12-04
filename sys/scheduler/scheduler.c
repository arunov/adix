#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/kstdio.h>
#include<sys/list.h>
#include<sys/memory/handle_cr.h>
#include<sys/memory/page_table_helper.h>
#include<sys/memory/sys_malloc.h>
#include<syscall.h>
LIST_HEAD(pcb_run_queue);
LIST_HEAD(pcb_terminated_queue);
LIST_HEAD(pcb_wait_queue);
LIST_HEAD(wait_timer_queue);

/* Get next task that is ready to be run*/
static struct pcb_t* getNextTask(){
	return list_entry(pcb_run_queue.next,struct pcb_t,lister);
}

static struct pcb_t *prepTask = NULL;

void updatePrepTask(struct pcb_t *pcb) {
    prepTask = pcb;
}

struct pcb_t* getCurrentTask(){

    if(prepTask) {
        return prepTask;
    }

    if(pcb_run_queue.prev == &pcb_run_queue) {
        return NULL;
    }

	return list_entry(pcb_run_queue.prev,struct pcb_t,lister);
}

struct pcb_t* getTerminatedTask(){
	if(list_empty(&pcb_terminated_queue)){
		return NULL;
	}
	return list_entry(pcb_terminated_queue.next,struct pcb_t,lister);
}

void sys_yield(){
#ifdef SCHED_DEBUG
	printf("\n#####Yielding to next process######");
#endif	
	struct pcb_t *nextTask = getNextTask();
	list_del(&nextTask->lister);//remove from head
	list_add_tail(&nextTask->lister,&pcb_run_queue); //add to tail
	if(nextTask->cr3_content != NULL){
		set_cr3(*(struct str_cr3*)(&nextTask->cr3_content));
	}
    if(nextTask->pid == 1 || nextTask->pid == 3) {
//    printf("nextTask: pid: %p, stack_base: %p, ", nextTask->pid, nextTask->stack_base);
//	printf("nextTask: pid %p rsp %p\n",nextTask->pid, get_u_rsp(nextTask));
    //printf("user_stack: %p, user_stack_phys: %p\n", nextTask->u_stack_base, virt2phys_selfref((uint64_t)nextTask->u_stack_base, NULL));
    //printf("nextTask: pid: %p, ", nextTask->pid);
    //printf("user_stack_phys: %p, ", virt2phys_selfref((uint64_t)nextTask->u_stack_base, NULL));
    //printf("k_stack_ptr: %p\n", nextTask->stack_base[1]);
    }
	switchTo(nextTask->stack_base);
}

static void handle_wakeup_on_exit(struct pcb_t *current_task){
	if(current_task->parent){ 
		/* This is a child process. Notify parent if its waiting for its child */
		int num_wakeups = sys_wakeup(current_task->pid); //if waiting on waitpid() syscall
		if(num_wakeups == 0){
			/* If someone was waiting for this process on waitpid, 
			 * no one should wait for this on wait, TODO: Needs surgery*/
			sys_wakeup(current_task->parent->pid);//if waiting on wait() syscall
		}
		/* parent has to return from wait with child's pid*/
		current_task->parent->wakeup_return = current_task->pid;
		remove_child(current_task);//Remove this child from its parent's child list
	}
	if(has_children(current_task)){
		/* This task is a parent. Inform its children about its exit */
		remove_parent_info_from_children(current_task);
	}
}

void sys_exit(int status){ 
	struct pcb_t *current_task = getCurrentTask();
	updateState(current_task, P_TERMINATED);
	list_del(&current_task->lister);//delete from run queue
	//add to terminated queue
	list_add(&current_task->lister,&pcb_terminated_queue);
	//wakeup any process waiting for it
	handle_wakeup_on_exit(current_task);
	sys_yield();//schedule next job
}

/* TODO: Write a swrapper sys_wait*/

int64_t sys_sleep(uint64_t wait_desc){
	struct pcb_t *current_task = getCurrentTask();
	update_wait_descriptor(current_task, wait_desc);
	list_del(&current_task->lister);
	list_add(&current_task->lister, &pcb_wait_queue);
	#ifdef SCHED_DEBUG
	printf("\n#####SLEEPING PROCESS %d",current_task->pid);
	#endif
	sys_yield();
	/* The process woke up here when somebody woke up this process. 
	 * That somebody is expected to populate the return value too!*/
	return current_task->wakeup_return;
}

int sys_sleep_timer(int64_t sleep_interval){
	if(sleep_interval <= 0){
		return -1;
	}
	struct pcb_t *current_task = getCurrentTask();
	current_task->sleep_time_rem = sleep_interval;
	list_del(&current_task->lister);
	list_add(&current_task->lister, &wait_timer_queue);
	sys_yield();
	return 0;
}

static void wakeup_proc(struct pcb_t *waiting_task){
	update_wait_descriptor(waiting_task, NOT_WAITING);
	list_del(&waiting_task->lister);//delete from wait queue
	list_add(&waiting_task->lister, &pcb_run_queue);//add to run queue
	#ifdef SCHED_DEBUG
	printf("\n#####WAKING UP process %d",waiting_task->pid);
	#endif
}


static int _sys_wakeup(struct list_head *wait_queue, uint64_t wait_desc){
	struct pcb_t *proc;
	struct pcb_t *temp[MAX_WAIT_PROC]; //TODO: Poses an upper bound?
	int i=0,c;
	list_for_each_entry(proc, wait_queue, lister){
		if(proc->wait_desc == wait_desc){
			temp[i++] = proc;
		}
	}
	c=i;
	while(i != 0){
		wakeup_proc(temp[--i]);
	}
	return c;
}

void sys_wakeup_timer(){
	struct pcb_t *proc;
	//struct list_head* head = &wait_timer_queue;
	list_for_each_entry(proc, &wait_timer_queue, lister){
		proc->sleep_time_rem -= 1;
		if(proc->sleep_time_rem == 0){
			update_wait_descriptor(proc, TIMER_ELAPSED);
		}
	}
	_sys_wakeup(&wait_timer_queue, TIMER_ELAPSED);
}

int sys_wakeup(uint64_t wait_desc){
	return _sys_wakeup(&pcb_wait_queue, wait_desc);
}

void cleanupTerminated(){
	struct pcb_t *terminated_task = getTerminatedTask();	
	if(terminated_task != NULL){
		list_del(&terminated_task->lister);
		//printf("\nContents of PCB after cleanup..");	
		printPcbRunQueue();
		deep_free_task(terminated_task);
	}
}

void addToTaskList(struct pcb_t *pcb){
	list_add(&pcb->lister,&pcb_run_queue);
}

void printPcbRunQueue(){
	struct pcb_t *the_pcb = NULL;
	#ifdef SCHED_DEBUG
	printf("\nPCB READY QUEUE: ");
	#endif
	list_for_each_entry(the_pcb,&pcb_run_queue,lister){
		printPcb(the_pcb);
	}
}

static struct ps_t* single_process_snapshot_copy(struct pcb_t *pcb,
                                                        enum ps_state a_state) {
    if(!pcb)
        return NULL;

    char *pname = NULL;
    if(pcb->name) {
        pname = (char*)sys_malloc(strlen(pcb->name) + 1);
        if(!pname)
            return NULL;
        memcpy(pname, pcb->name, strlen(pcb->name) + 1);
    }

    struct ps_t *ps1 = (struct ps_t*)sys_malloc(sizeof(struct ps_t));
    if(!ps1) {
        sys_free(pname);
        return NULL;
    }
    ps1->pid = pcb->pid;
    ps1->name = pname;
    ps1->state = a_state;
    ps1->next = NULL;

    return ps1;
}

void sys_process_snapshot(struct ps_t **list) {
    if(!list)
        return;

    struct ps_t *last = NULL, *curr = NULL;
	struct pcb_t *the_pcb = NULL;

    list_for_each_entry(the_pcb, &pcb_run_queue, lister) {
        curr = single_process_snapshot_copy(the_pcb, RUNNING);
        if(last) {
            last->next = curr;
        } else {
            *list = curr;
        }
        last = curr;
    }

    list_for_each_entry(the_pcb, &pcb_wait_queue, lister) {
        curr = single_process_snapshot_copy(the_pcb, WAITING);
        if(last) {
            last->next = curr;
        } else {
            *list = curr;
        }
        last = curr;
    }

    list_for_each_entry(the_pcb, &wait_timer_queue, lister) {
        curr = single_process_snapshot_copy(the_pcb, WAITING_TIMER);
        if(last) {
            last->next = curr;
        } else {
            *list = curr;
        }
        last = curr;
    }
}

