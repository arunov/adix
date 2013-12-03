#ifndef PCB_H
#define PCB_H
#include <defs.h>
#include<sys/list.h>
#include <sys/limits.h>
#define NUM_REGISTERS_BACKED 15
#define STACK_SIZE 512
#define SP_OFFSET 1

#define ULIMIT 1000
#define TIMER_ELAPSED ULIMIT+1

enum pstate{
	P_READY,	/* Process ready */
	P_TERMINATED	/* Terminated */
};

enum ptype{
	KTHREAD,	/* Kernel Thread */
	UPROC		/* User process */
};

struct pcb_t{
	uint64_t *stack_base;
	uint64_t *u_stack_base;
	uint64_t pid;
	struct pcb_t *parent; 
	int64_t wakeup_return;//TODO: Remove
	struct list_head *children;
	enum pstate state;
	enum ptype type;
	uint64_t cr3_content;
	struct tss_t *tss;
	/* Indicates which descriptor the process is sleeping on */
	uint64_t wait_desc; 
	uint64_t sleep_time_rem;//TODO:Handle sleep through signals!
	struct  process_files_table* open_files[OPEN_FILES_LIMIT];
	struct list_head lister; 
	struct list_head child_lister; 
	struct mm_struct *mm;
};

/* Create a new Task(PCB) and initialize its values.*/
struct pcb_t* createTask(enum ptype proc_type,
			uint64_t instruction, 
			char *program,
			void *kernmem,
			void *physfree);
/* Update process state */			
void updateState(struct pcb_t *this, enum pstate);
/* Update wait descriptor that process is waiting for*/
void update_wait_descriptor(struct pcb_t *this, uint64_t wait_desc);
void printPcb(struct pcb_t *this);
uint64_t get_u_rsp(struct pcb_t *this);
uint64_t get_u_rip(struct pcb_t *this);

/* Get the file descriptor object at the given fd */
struct process_files_table* get_process_files_table(
				struct pcb_t *this,
				int fd);
/* Add an entry into process's file descriptor table */				
uint64_t add_to_process_file_table(
				struct pcb_t *this,
				struct process_files_table *pft);
/* Delete an entry from process file descriptor table */
uint64_t reset_process_files_table( struct pcb_t *this,
				uint64_t fd);

/**
 * Fork system call
 * @return pid of child
 */
uint64_t sys_fork();
int has_children();
void add_child(struct pcb_t *this, struct pcb_t *c_pcb);
void remove_child(struct pcb_t *c_pcb);
void remove_parent_info_from_children(struct pcb_t *this);
#endif

