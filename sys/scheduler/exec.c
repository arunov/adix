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
#include <sys/memory/handle_cr.h>
#include <sys/ulimit/sys_ulimit.h>

void* next_available_address = NULL;

uint64_t get_argc(char *argv[]){
	int i =0;
	if(argv == NULL) return 0;
	for(i=0; argv[i] != NULL; i++);
	return i;
}

char** dup_argv(uint64_t argc, char **argv, void*(*alloc)(uint64_t)){
	int i,len;
	//you need extra one pointer to mark the end
	char **uargv = (char**)alloc((argc+1) * sizeof(char *));
	for(i=0; i<argc; i++){
		len = strlen(argv[i]);
		//allocate memory for the string itself
		uargv[i] = alloc(len+1);
		//copy the contents from userspace1 to userspace 2
		memcpy(uargv[i], argv[i], len+1);
	}
	uargv[i] = NULL;
	return uargv;
}

char** get_userspace_env(char *envp[]){
	return NULL; //TODO
}

struct userspace_args* dup_args(char **argv, char **envp,void*(*alloc)(uint64_t)){
	struct userspace_args *ua = (struct userspace_args*) alloc(sizeof(struct userspace_args)); 
	if(ua == NULL){
		return NULL;
	}
	ua->argc = get_argc(argv);
	ua->argv = dup_argv(ua->argc, argv, alloc);
	int envc = get_argc(envp);
	ua->envp = dup_argv(envc, envp, alloc);
	return ua;
}

uint64_t sys_execvpe(char *path, char *argv[], char *envp[]){
	//replace virtual adress space
	 //copy args to kernel space
	 struct userspace_args* kargs = dup_args(argv, envp, kmalloc);
	 struct pcb_t *pcb = getCurrentTask();

     // Change process name
     //char *old_name = pcb->name;
     pcb->name = NULL;
     if(path) {
         pcb->name = (char*)kmalloc(strlen(path) + 1);
         if(!pcb->name) {
            kfree(pcb);
            return -1;
         }
         memcpy(pcb->name, path, strlen(path) + 1);
     }

	 struct mm_struct *old_mm = pcb->mm;
     uint64_t pml4_phys = 0;
     if(0 == get_kduplicate_curr_self_ref_PML4(&pml4_phys)) {
        return -1;
     }

     uint64_t old_cr3_content = pcb->cr3_content;
     pcb->cr3_content = pml4_phys;
     set_cr3(*(struct str_cr3*)(&pml4_phys));

	 pcb->mm = new_mm();
	 //Load binary
	 uint64_t rip = load_elf(pcb->mm, pcb->name);
	 if(rip == -1){ /* Loading of elf failed */
	 	kfree(pcb->mm);//TODO: Deep free?
	 	pcb->mm = old_mm;
        pcb->cr3_content = old_cr3_content;
        set_cr3(*(struct str_cr3*)(&old_cr3_content));
		return 0;
	 }
	 /* Initalize stack only if load went through, otherwise virtual address for
	  * the stack will be remapped and all the contents gone, causing sigsegv! */
	 pcb->u_stack_base = (uint64_t*)v_alloc_pages_at_virt(1, 
	 							PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR, 
								0x70000000);
     struct vm_area_struct *u_stack_vma = find_vma(&(pcb->mm->mmap), (uint64_t)pcb->u_stack_base);
     // TODO: Check NULL
     u_stack_vma->vm_flags |= MAP_GROWSDOWN;
     u_stack_vma->max_size = rlimit_cur[RLIMIT_STACK]/SIZEOF_PAGE;

	 //Get user stack location to jump to
	 uint64_t rsp = ((uint64_t)pcb->u_stack_base) | 0xfff;
	 //Copy args into user space
	 struct userspace_args* uargs = dup_args(kargs->argv, kargs->envp, sys_malloc);
	 _jump_to_ring3(rsp,rip, uargs->argc, uargs->argv, uargs->envp);
	 kfree(kargs);
	return 0;
}
