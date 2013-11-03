#ifndef PCB_H
#define PCB_H
#include <defs.h>
#include<sys/list.h>
#define NUM_REGISTERS_BACKED 14
#define STACK_SIZE 512
#define SP_OFFSET 1
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
	uint64_t pid;
	enum pstate state;
	enum ptype type;
	struct list_head lister; 
};

struct pcb_t* createTask(enum ptype proc_type,uint64_t instruction);
void updateState(struct pcb_t *this, enum pstate);
void printPcb(struct pcb_t *this);
#endif
