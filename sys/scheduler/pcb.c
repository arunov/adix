#include<sys/kstdio.h>
#include<sys/scheduler/pcb.h>
#include<sys/memory/virtual_page_manager.h>
#define NUM_REGISTERS 14
#define STACK_SIZE 511
#define SP_OFFSET 1

uint64_t next_pid = 1;
/* Todo: Temporary */
struct pcb_t pcb_a[2];
int counter = 0;

uint64_t getNextPid(){ //method not exposed through interface
	return next_pid++;
}

void initStackForContextSwitch(uint64_t *stack, uint64_t instruction){
	stack[STACK_SIZE] = instruction; 
	printf("Location where instruction is copied:%x,%x\n",
				&stack[STACK_SIZE],
				stack[STACK_SIZE]);
	//Save stack pointer for the new stack
	stack[SP_OFFSET] = (uint64_t)&stack[STACK_SIZE - NUM_REGISTERS]; 
	
}

struct pcb_t* createTask(enum ptype proc_type, uint64_t instruction_address){
	printf("creating Task\n");
	//Change once memory allocator is implemented
	struct pcb_t *pcb = &(pcb_a[counter++%2]);
	pcb->stack_base = getFreeVirtualPage();
	printf("Position of stack base:%x",pcb->stack_base);
	pcb->pid = getNextPid();
	pcb->state = P_READY;
	pcb->type = proc_type;
	
	initStackForContextSwitch(pcb->stack_base,instruction_address);
	return pcb;
}

void updateState(struct pcb_t *this, enum pstate state){
	this->state = state;
}

void saveProcessState(struct pcb_t *this){
	return;
}


