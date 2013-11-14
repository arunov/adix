#include<sys/kstdio.h>
#include<sys/scheduler/pcb.h>
#include<sys/memory/virtual_page_manager.h>
#include<sys/memory/setup_kernel_pgtbl.h> //TODO:Remove after 
#include<sys/scheduler/ring_switch.h> //TODO:Remove after 

//TODO:Remove after allocator
#define NUM_TASKS 3
extern char physbase;
static uint64_t next_pid = 1;
/* Todo: Temporary */
struct pcb_t pcb_a[NUM_TASKS];
int counter = 0;

/* Get next available value for process id. This method should be used
   within this class only.*/
static uint64_t getNextPid(){ 
	return next_pid++;
}

uint64_t get_u_rsp(struct pcb_t *this){
	return this->u_stack_base[SP_OFFSET];
}

uint64_t get_u_rip(struct pcb_t *this){
	uint64_t *rip_loc = (&((uint64_t*)this->u_stack_base[SP_OFFSET])[NUM_REGISTERS_BACKED]); 
	printf("\nRIP location:%p",rip_loc);
	return *rip_loc;
}


/* Allocate memory for a PCB. */
struct pcb_t* newPcb(){
	//TODO:Change once memory allocator is implemented
	return &(pcb_a[counter++%NUM_TASKS]);
}

/* Scheduler expects that each process stack contain information for its 
 * operation such as saved registers, sp and instruction to jump to(rip).
 * This method initializes these values
*/
static void prepareInitialStack(uint64_t *stack, uint64_t ip_val){
	stack[STACK_SIZE-1] = ip_val; 
	printf("Location where instruction is copied:%x,%x\n",//TODO:Debug
				&(stack[STACK_SIZE]),
				stack[STACK_SIZE]);
	//Save stack pointer for the new stack
	stack[SP_OFFSET] = (uint64_t)&stack[STACK_SIZE-1-NUM_REGISTERS_BACKED]; 
}


struct pcb_t* createTask(enum ptype proc_type, 
			uint64_t instruction_address, 
			void *kernmem,
			void *physfree){
	printf("creating Task\n");
	struct pcb_t *pcb = newPcb();
	pcb->pid = getNextPid();
	pcb->state = P_READY;
	pcb->type = proc_type;
	struct str_cr3 cr3 = create_kernel_pgtbl(kernmem, &physbase, physfree); 
	pcb->cr3_content = *((uint64_t*)&cr3); // TODO: Replace with loader load of binary
	//Prepare stack for the initial context switch
	if(proc_type == KTHREAD){
		pcb->stack_base = getFreeVirtualPage();
		prepareInitialStack(pcb->stack_base,instruction_address);
	} else if(proc_type == UPROC){
		pcb->stack_base = getFreeVirtualPage();
		pcb->u_stack_base = getFreeVirtualPage();
		prepareInitialStack(pcb->stack_base,(uint64_t)&jump_to_ring3);
		prepareInitialStack(pcb->u_stack_base, instruction_address);
	}
	return pcb;
}

/* Update task state for the PCB passed as an argument*/
void updateState(struct pcb_t *this, enum pstate state){
	this->state = state;
}

/* Print contents of the PCB passed as an argument.*/
void printPcb(struct pcb_t *this){
	printf("\n---PCB---");
	printf("{Kernel Stack=%p},{User stack=%p},{pid=%d},{pstate=%d},{ptype=%d}",
		this->stack_base,
		this->u_stack_base,
		this->pid,
		this->state,
		this->type);
	printf("---E_PCB---");
}
