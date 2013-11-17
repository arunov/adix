#include <defs.h>
#include <syscall.h>
#include <sys/irq.h>
#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/syscall/syscall_handler.h>

typedef void* (sys_call_t)(void*) ;
sys_call_t *sys_call_table[NUM_SYS_CALLS] = {
	[YIELD] = (sys_call_t*)_sys_yield,
	[PRINTF] = (sys_call_t*)_sys_printf,
	[EXIT] = (sys_call_t*)_sys_exit
};
