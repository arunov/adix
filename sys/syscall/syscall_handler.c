#include <defs.h>
#include <syscall.h>
#include <sys/irq.h>
#include <sys/kstdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/scheduler/scheduler_utils.h>
#include <sys/syscall/syscall_handler.h>
#include <sys/parser/parsetarfs.h>
#include <sys/memory/sys_malloc.h>
#include <sys/scheduler/exec.h>
#include <sys/scheduler/pcb.h>

typedef void* (sys_call_t)(void*) ;

int64_t sys_read_stub(int fd, void *buf, uint64_t count){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	 return	pft->op_pointers->read(
		fd,
		buf,
		count);
}

int64_t sys_write_stub(int fd, void *buf, uint64_t count){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	return pft->op_pointers->write(
		fd,
		buf,
		count);
	
}


int sys_lseek_stub(int fd, off64_t offset, int whence){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	return pft->op_pointers->lseek(
		fd,
		offset,
		whence);

}

int sys_close_stub(int fd){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	return pft->op_pointers->close(
		fd);

}

struct posix_header_ustar* sys_readdir_stub(int fd, uint64_t ret){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	return pft->op_pointers->readdir(
		fd,
		ret);

}

int sys_closedir_stub(int fd){
	 struct process_files_table *pft = get_process_files_table(getCurrentTask(),fd);
	return pft->op_pointers->closedir(
		fd);

}

sys_call_t *sys_call_table[NUM_SYS_CALLS] = {
	[YIELD] = (sys_call_t*)_sys_yield,
	[PRINTF] = (sys_call_t*)_sys_printf,
	[EXIT] = (sys_call_t*)_sys_exit,
	[OPEN] = (sys_call_t*)_sys_open,
	[READ] = (sys_call_t*)sys_read_stub,
	[LSEEK] = (sys_call_t*)sys_lseek_stub,
	[CLOSE] = (sys_call_t*)sys_close_stub,
	[OPENDIR] = (sys_call_t*)_sys_opendir,
	[READDIR] = (sys_call_t*)sys_readdir_stub,
	[CLOSEDIR] = (sys_call_t*)sys_closedir_stub,
	[SLEEP] = (sys_call_t*)_sys_sleep,
	[WRITE] = (sys_call_t*)sys_write_stub,
	[CLRSCR] = (sys_call_t*)_sys_clrscr,
	[MALLOC] = (sys_call_t*)sys_malloc,
	[EXEC] = (sys_call_t*)sys_execvpe,
	[GETPID] = (sys_call_t*)sys_getpid,
    [FORK] = (sys_call_t*)_sys_fork,
    [WAITPID] = (sys_call_t*)sys_sleep,
};

