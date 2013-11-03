#include<sys/kstdio.h>
#include<sys/scheduler/pcb.h>
#include<sys/memory/virtual_page_manager.h>

//TODO:Remove after allocator
#define NUM_TASKS 3

static uint64_t next_pid = 1;
/* Todo: Temporary */
struct pcb_t pcb_a[NUM_TASKS];
int counter = 0;

/* Get next available value for process id. This method should be used
   within this class only.*/
static uint64_t getNextPid(){ 
	return next_pid++;
}

/* Scheduler expects that each process stack contain information for its 
   operation such as saved registers, sp and instruction to jump to(rip).
   This method initializes these values*/
void initStackForContextSwitch(struct pcb_t *this, uint64_t ip_val){
	uint64_t *stack = this->stack_base;
	stack[STACK_SIZE-1] = ip_val; 
	printf("Location where instruction is copied:%x,%x\n",//TODO:Debug
				&(stack[STACK_SIZE]),
				stack[STACK_SIZE]);
	//Save stack pointer for the new stack
	stack[SP_OFFSET] = (uint64_t)&stack[STACK_SIZE-1-NUM_REGISTERS_BACKED]; 
}

/* Allocate memory for a PCB. */
struct pcb_t* newPcb(){
	//TODO:Change once memory allocator is implemented
	return &(pcb_a[counter++%NUM_TASKS]);
}

/* Create a new Task(PCB) and initialize its values.*/
struct pcb_t* createTask(enum ptype proc_type, uint64_t instruction_address){
	printf("creating Task\n");
	struct pcb_t *pcb = newPcb();
	pcb->stack_base = getFreeVirtualPage();
	pcb->pid = getNextPid();
	pcb->state = P_READY;
	pcb->type = proc_type;
	initStackForContextSwitch(pcb,instruction_address);
	return pcb;
}

/* Update task state for the PCB passed as an argument*/
void updateState(struct pcb_t *this, enum pstate state){
	this->state = state;
}

/* Print contents of the PCB passed as an argument.*/
void printPcb(struct pcb_t *this){
	printf("\n---PCB---");
	printf("{Stack Base=%p},{pid=%d},{pstate=%d},{ptype=%d}",
		this->stack_base,
		this->pid,
		this->state,
		this->type);
	printf("---E_PCB---");
}
