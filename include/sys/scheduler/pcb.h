#ifndef PCB_H
#define PCB_H
#include <defs.h>

enum pstate{
	P_READY,	/* Process ready */
	P_RUNNING,	/* Running */
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
};

struct pcb_t* createTask(enum ptype proc_type,uint64_t instruction);
void updateState(struct pcb_t *this, enum pstate);
void saveProcessState(struct pcb_t *this);
#endif
