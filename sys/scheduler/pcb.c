#include <stdio.h>
#include <string.h>
#include<sys/kstdio.h>
#include<sys/scheduler/pcb.h>
#include<sys/memory/virtual_page_manager.h>
#include<sys/memory/setup_kernel_pgtbl.h> //TODO:Remove after 
#include<sys/memory/page_table_helper.h> //TODO:Remove after 
#include<sys/scheduler/ring_switch.h> //TODO:Remove after 
#include <sys/memory/kmalloc.h>
#include <sys/parser/parseelf.h>
#include <sys/gdt.h>
#include <sys/filesystems/file_structures.h>
#include <sys/terminal/terminal_driver.h>
#include <sys/memory/page_constants.h>
#include <sys/memory/free_phys_pages.h>
#include <sys/scheduler/scheduler.h>

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

static struct list_head* init_children_list(){
	 struct list_head *lh = kmalloc(sizeof( struct list_head));
	 lh->next = lh;
	 lh->prev = lh;
	 return lh;

}

struct pcb_t* createTask(enum ptype proc_type, 
			uint64_t instruction_address, 
			char *program,
			void *kernmem,
			void *physfree){
	printf("creating Task\n");
	struct pcb_t *pcb = newPcb();
	pcb->pid = getNextPid();
	pcb->parent = NULL;
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
    pcb->mm = new_mm();
	pcb->children = init_children_list();
    pcb->name = NULL;
    if(program) {
        pcb->name = (char*)kmalloc(strlen(program) + 1);
        memcpy(pcb->name, program, strlen(program) + 1);
    }
    updatePrepTask(pcb);
	//Prepare stack for the initial context switch
	if(proc_type == KTHREAD){
		pcb->stack_base = (uint64_t*)v_alloc_pages(1, PAGE_TRANS_READ_WRITE);
		prepareInitialStack(pcb->stack_base,instruction_address);
		pcb->tss = (struct tss_t*)kmalloc(sizeof(struct tss_t));
	} 
	else if(proc_type == UPROC){
		/* It is sufficient to use different CR3 for user processes! (is it?)*/
		/* Load the binary corresponding to this process */
		struct str_cr3 cr3 = create_kernel_pgtbl(kernmem, &physbase, physfree); 
		pcb->cr3_content = *((uint64_t*)&cr3);
		set_cr3(cr3);
		//load elf
		instruction_address = load_elf(pcb->mm, program);
		pcb->tss = (struct tss_t*)kmalloc(sizeof(struct tss_t));
		pcb->stack_base = (uint64_t*)v_alloc_pages(1, PAGE_TRANS_READ_WRITE);
		pcb->u_stack_base = (uint64_t*)v_alloc_pages_at_virt(1, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR, 0x70000000);
		printf("##########stack alocated for user process: %p",pcb->u_stack_base);
		prepareInitialStack(pcb->stack_base,(uint64_t)&jump_to_ring3);
		prepareInitialStack(pcb->u_stack_base, instruction_address);
	}
    updatePrepTask(NULL);
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

#define SYS_FORK_CHILD_RIP_OFFSET 0x28

uint64_t sys_fork() {

    // Get rsp of parent
    static uint64_t p_rsp;

	__asm volatile(
		"movq %%rsp, %0;"
		:"=g"(p_rsp)
		:
	);

    uint64_t c_rsp = p_rsp + SYS_FORK_CHILD_RIP_OFFSET; 

    struct pcb_t *p_pcb = getCurrentTask();

    // PCB to return
    struct pcb_t *c_pcb = (struct pcb_t*)kmalloc(sizeof(struct pcb_t));
    if(NULL == c_pcb) {
        return NULL;
    }

    /* stack_base */
    c_pcb->stack_base = (uint64_t*)v_alloc_pages(1, PAGE_TRANS_READ_WRITE);
    if(c_pcb->stack_base == NULL) {
        kfree(c_pcb);
        return NULL;
    }

    // Copy stack
    memcpy((void*)((char*)(c_pcb->stack_base) + OFFSET_IN_PAGE(c_rsp)),
                    (void*)((char*)(p_pcb->stack_base) + OFFSET_IN_PAGE(c_rsp)),
                                        SIZEOF_PAGE - OFFSET_IN_PAGE(c_rsp));
    memset((void*)(c_pcb->stack_base), 0, OFFSET_IN_PAGE(c_rsp));

    c_pcb->stack_base[SP_OFFSET] = (uint64_t)(c_pcb->stack_base)
            + OFFSET_IN_PAGE(c_rsp) - sizeof(uint64_t) * NUM_REGISTERS_BACKED;

    /* u_stack_base */
    c_pcb->u_stack_base = p_pcb->u_stack_base;

    /* pid */
    c_pcb->pid = getNextPid();

    /* name */
    c_pcb->name = NULL;
    if(p_pcb->name) {
        c_pcb->name = (char*)kmalloc(strlen(p_pcb->name) + 1);
        if(!c_pcb->name) {
            kfree(c_pcb);
            return NULL;
        }
        memcpy(c_pcb->name, p_pcb->name, strlen(p_pcb->name) + 1);
    }

	/* parent pid */
	c_pcb->parent = p_pcb;//TODO: What will happen if parent exited before child?
	c_pcb->children = init_children_list();
	add_child(p_pcb, c_pcb);

    /* state */
    c_pcb->state = p_pcb->state;

    /* type */
    c_pcb->type = p_pcb->type;

    /* cr3_content */
    if(0 == cow_fork_page_table(&(c_pcb->cr3_content))) {
        // TODO: Deep free!
        kfree(c_pcb);
        return NULL;
    }

    /* tss */
    c_pcb->tss = (struct tss_t*)kmalloc(sizeof(struct tss_t));
    if(NULL == c_pcb->tss) {
        // TODO: Deep free!
        kfree(c_pcb);
        return NULL;
    }

    /* wait_desc */
    c_pcb->wait_desc = p_pcb->wait_desc;

    /* open_files */
    for(int i = 0; i < OPEN_FILES_LIMIT; i++) {
        if(p_pcb->open_files[i]) {
            c_pcb->open_files[i] =
                        get_duplicate_process_files_table(p_pcb->open_files[i]);
            continue;
        }
        c_pcb->open_files[i] = NULL;
    }

    /* mm */
    c_pcb->mm = cow_fork_mm_struct(p_pcb->mm);
    if(NULL == c_pcb->mm) {
        // TODO: Deep free!
        kfree(c_pcb);
        return NULL;
    }

    /* lister */
    addToTaskList(c_pcb);

    return c_pcb->pid;
}

int has_children(struct pcb_t *pcb){
	struct list_head *lh = pcb->children;
	if(lh->next == lh && lh->prev == lh){
		return 0; //no children
	}
	return 1;
}

void add_child(struct pcb_t *this, struct pcb_t *c_pcb){
	list_add_tail(&c_pcb->child_lister, this->children); 
}

void remove_child(struct pcb_t *c_pcb){
	list_del(&c_pcb->child_lister);	
}

void remove_parent_info_from_children(struct pcb_t *this){
	struct pcb_t *pcb;
	list_for_each_entry(pcb, this->children, child_lister){
		/* Reset parent field for each of the children. 
		 * Called when parent exits! */
		//TODO: Mark them as zombies! And do what? GOD knows!
		pcb->parent = NULL;
	}

}

void deep_free_task(struct pcb_t *this){
	//TODO: Deep free
	kfree(this->children);
	kfree(this->tss);
	kfree(this);
}
