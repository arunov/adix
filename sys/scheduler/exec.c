#include <sys/scheduler/scheduler.h>
#include <sys/scheduler/ring_switch.h>
#include <string.h>
#include <stdio.h>
#include <sys/scheduler/exec.h>
#include <sys/scheduler/pcb.h>
#include <sys/parser/parseelf.h>
#include <sys/memory/kmalloc.h>
#include <sys/memory/sys_malloc.h>
#include <sys/memory/free_phys_pages.h>

void* next_available_address = NULL;

void my_malloc_init(){
	next_available_address = (void*)(&(getCurrentTask()->u_stack_base[10]));
}

uint64_t get_argc(char *argv[]){
	int i =0;
	if(next_available_address == NULL){
		my_malloc_init();
	}
	if(argv == NULL) return 0;
	for(i=0; argv[i] != NULL; i++);

	return i;
}

//TODO: Remove
void* my_malloc(uint64_t size){
	void* address = next_available_address;
	next_available_address += size;
	return address;
}

char** dup_argv(uint64_t argc, char **argv, void*(*alloc)(uint64_t)){
	int i,len;
	char **uargv = (char**)alloc(argc * sizeof(char *));
	for(i=0; i<argc; i++){
		len = strlen(argv[i]);
		//allocate memory for the string itself
		uargv[i] = alloc(len+1);
		//copy the contents from userspace1 to userspace 2
		memcpy(uargv[i], argv[i], len+1);
	}
	return uargv;
}

char** get_userspace_env(char *envp[]){
	return NULL; //TODO
}

struct userspace_args* dup_args(char **argv, char **envp,void*(*alloc)(uint64_t)){
	struct userspace_args *ua = (struct userspace_args*) alloc(sizeof(struct userspace_args)); 
	ua->argc = get_argc(argv);
	ua->argv = dup_argv(ua->argc, argv, alloc);
	ua->envp = get_userspace_env(envp);
	return ua;
}

uint64_t sys_execvpe(char *path, char *argv[], char *envp[]){
	//replace virtual adress space
	 //copy args to kernel space
	 struct userspace_args* kargs = dup_args(argv, envp, kmalloc);
	 struct pcb_t *pcb = getCurrentTask();
	 struct mm_struct *old_mm = pcb->mm;
	 pcb->mm = new_mm();
	 //Load binary
	 uint64_t rip = load_elf(pcb->mm, path);
	 if(rip == -1){ /* Loading of elf failed */
	 	kfree(pcb->mm);//TODO: Deep free?
	 	pcb->mm = old_mm;
		return 0;
	 }
	 /* Initalize stack only if load went through, otherwise virtual address for
	  * the stack will be remapped and all the contents gone, causing sigsegv! */
	 pcb->u_stack_base = (uint64_t*)v_alloc_pages_at_virt(1, 
	 							PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR, 
								0x70000000);
	 //Get user stack location to jump to
	 uint64_t rsp = ((uint64_t)pcb->u_stack_base) | 0xfff;
	 //Copy args into user space
	 struct userspace_args* uargs = dup_args(kargs->argv, kargs->envp, sys_malloc);
	 _jump_to_ring3(rsp,rip, uargs->argc, uargs->argv, uargs->envp);
	 kfree(kargs);
	return 0;
}
