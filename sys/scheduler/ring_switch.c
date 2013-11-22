#include<sys/scheduler/ring_switch.h>
#include<sys/kstdio.h>
#include<defs.h>
#include<sys/scheduler/pcb.h>
#include<sys/scheduler/scheduler.h>
#include <sys/gdt.h>
void jump_to_ring3(){
	struct pcb_t *pcb = getCurrentTask();
	uint64_t rsp = get_u_rsp(pcb);
	uint64_t rip = get_u_rip(pcb);
	printf("JUMP_TO_RING3: rsp: %p rip: %p",rsp,rip);
	_jump_to_ring3(rsp,rip);

}

void set_tss(uint64_t rsp){
	configure_tss_in_gdt(getCurrentTask()->tss, rsp);
}


