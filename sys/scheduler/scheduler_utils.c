#include <sys/scheduler/scheduler_utils.h>
#include <sys/scheduler/scheduler.h>
#include <defs.h>

uint64_t sys_getpid(){
	return getCurrentTask()->pid;
}
