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
	char* argv[6] = {"pgm_name","first_param","second_param",};//replaced
	char* envp[5] = {"othing"};
	argv[3] = 0;
	envp[1] = 0;
	 struct userspace_args ua;

	 ua = prepare_args_for_userspace(argv, envp);//replaced
	 printf("\nJUMP_TO_RING3: rsp: %p rip: %p argc:%d argv:%p\n",rsp,rip, ua.argc, ua.argv);
	_jump_to_ring3(rsp, rip, ua.argc, ua.argv, ua.envp);

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


