#include<sys/scheduler/ring_switch.h>
#include<sys/kstdio.h>
#include<defs.h>
#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include<sys/scheduler/exec.h>
#include <sys/gdt.h>

 struct userspace_args  prepare_args_for_userspace(char **argv, char **envp);
void jump_to_ring3(){
	struct pcb_t *pcb = getCurrentTask();
	uint64_t rsp = get_u_rsp(pcb);
	uint64_t rip = get_u_rip(pcb);
	printf("\n Jumping to ring3\n");
	_jump_to_ring3(rsp, rip, 1, 0, NULL);

}

void exec_jump_to_ring3(int argc, char *argv[], char *envp[]){
	
	printf("\nEXECUTING program %s, with args:",argv[0]);
	for( int i=0; i<argc; i++){
		printf("\n Arg %d : %s",argv[i]);
	}
	
}

void set_tss(uint64_t rsp){
	configure_tss_in_gdt(getCurrentTask()->tss, rsp);
}


