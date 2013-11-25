#include <stdio.h>
#include<sys/kstdio.h>
#include<sys/scheduler/pcb.h>
#include<sys/memory/virtual_page_manager.h>
#include<sys/memory/setup_kernel_pgtbl.h> //TODO:Remove after 
#include<sys/memory/page_table_helper.h> //TODO:Remove after 
#include<sys/scheduler/ring_switch.h> //TODO:Remove after 
#include <sys/memory/kmalloc.h>
#include <sys/parser/exec.h>
#include <sys/gdt.h>
#include <sys/filesystems/file_structures.h>
#include <sys/terminal/terminal_driver.h>
extern char physbase;
static uint64_t next_pid = 1;

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
	return (struct pcb_t*)kmalloc(sizeof(struct pcb_t));
}

/* Scheduler expects that each process stack contain information for its 
 * operation such as saved registers, sp and instruction to jump to(rip).
 * This method initializes these values
*/
static void prepareInitialStack(uint64_t *stack, uint64_t ip_val){
	stack[STACK_SIZE-1] = ip_val; 
	//Save stack pointer for the new stack
	stack[SP_OFFSET] = (uint64_t)&stack[STACK_SIZE-1-NUM_REGISTERS_BACKED]; 
}


struct pcb_t* createTask(enum ptype proc_type, 
			uint64_t instruction_address, 
			char *program,
			void *kernmem,
			void *physfree){
	printf("creating Task\n");
	struct pcb_t *pcb = newPcb();
	pcb->pid = getNextPid();
	pcb->state = P_READY;
	pcb->type = proc_type;
	pcb->open_files[STDIN] = get_new_process_files_table(
					NULL,
					0,
					get_terminal_ops());
	pcb->open_files[STDOUT] = get_new_process_files_table(
					NULL,
					0,
					get_terminal_ops());
	//Prepare stack for the initial context switch
	if(proc_type == KTHREAD){
		pcb->stack_base = getFreeVirtualPage();
		prepareInitialStack(pcb->stack_base,instruction_address);
	} 
	else if(proc_type == UPROC){
		/* It is sufficient to use different CR3 for user processes!*/
		struct str_cr3 cr3 = create_kernel_pgtbl(kernmem, &physbase, physfree); 
		pcb->cr3_content = *((uint64_t*)&cr3); // TODO: Replace with loader load of binary
		set_cr3(cr3);
		/* Load the binary corresponding to this process */
		instruction_address = exec(program);
		pcb->tss = (struct tss_t*)kmalloc(sizeof(struct tss_t));
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

void update_wait_descriptor(struct pcb_t *this, uint64_t wait_desc){
	this->wait_desc = wait_desc;
}

/* Print contents of the PCB passed as an argument.*/
void printPcb(struct pcb_t *this){
#if 0
	printf("\n---PCB---");
	printf("{Kernel Stack=%p},{User stack=%p},{pid=%d},{pstate=%d},{ptype=%d},{cr3=%p}",
		this->stack_base,
		this->u_stack_base,
		this->pid,
		this->state,
		this->type,
		this->cr3_content);
	printf("---E_PCB---"); 
#endif	
}

struct process_files_table* get_process_files_table(
				struct pcb_t *this,
				int fd)
{
	return this->open_files[fd];

}

uint64_t add_to_process_file_table(
				struct pcb_t *this,
				struct process_files_table *pfd)
{
	uint64_t i;
	for(i =0; i<OPEN_FILES_LIMIT; i++){
		//search next available slot
		if(this->open_files[i] == 0){
			this->open_files[i] = pfd;
			break;
		}
	}
	if(i == OPEN_FILES_LIMIT){
		printf(" Number of open files exceeds OPEN_FILES_LIMIT");
		return -1;
	}
	return i;
}

uint64_t reset_process_files_table( struct pcb_t *this,
				uint64_t fd){
	struct process_files_table* pft = get_process_files_table(this,fd);
	if(pft == NULL){
		return -1;
	}
	kfree(get_process_files_table(this,fd));
	this->open_files[fd] = NULL;
	return 0;
}				

