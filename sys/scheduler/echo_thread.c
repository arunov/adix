#include <stdio.h>
#include <sys/scheduler/scheduler.h>
#include <sys/memory/kmalloc.h>
#include <sys/terminal/terminal_driver.h>
int64_t sys_write_stub(int fd, void *buf, uint64_t count);
int64_t sys_read_stub(int fd, void *buf, uint64_t count);

void invokeEcho(){
	void* buf = kmalloc(100);
	while(1){
		sys_read_stub(0, buf, 100);
		printf("\nECHO O/P : ");
		sys_write_stub(0, buf, strlen((char*)buf));
	}
	sys_exit(0);
}
