#include <sys/terminal/terminal_driver.h>
#include <sys/scheduler/scheduler.h>
#include <sys/memory/kmalloc.h>
void invokeEcho(){
	void* buf = kmalloc(100);
	while(1){
		terminal_read(0, buf, 100);
		printf("\nECHO O/P : ");
		terminal_write(0, buf, strlen((char*)buf));
	}
	sys_exit(0);
}
