#include <defs.h>
#include <syscall.h>
#include <sys/irq.h>
#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/syscall/syscall_handler.h>
#include <sys/parser/parsetarfs.h>

typedef void* (sys_call_t)(void*) ;
sys_call_t *sys_call_table[NUM_SYS_CALLS] = {
	[YIELD] = (sys_call_t*)_sys_yield,
	[PRINTF] = (sys_call_t*)_sys_printf,
	[EXIT] = (sys_call_t*)_sys_exit,
	[OPEN] = (sys_call_t*)_sys_open,
	[READ] = (sys_call_t*)_sys_read,
	[LSEEK] = (sys_call_t*)_sys_lseek,
	[CLOSE] = (sys_call_t*)_sys_close,
	[OPENDIR] = (sys_call_t*)_sys_opendir,
	[READDIR] = (sys_call_t*)_sys_readdir,
	[CLOSEDIR] = (sys_call_t*)_sys_closedir,
	[SLEEP] = (sys_call_t*)_sys_sleep
};
