#include <sys/scheduler/exec.h>
#include <sys/scheduler/scheduler.h>
#include <sys/scheduler/ring_switch.h>
#include <string.h>
#include <stdio.h>
#include <sys/scheduler/exec.h>

void* next_available_address = NULL;

void my_malloc_init(){
	next_available_address = (void*)(&(getCurrentTask()->u_stack_base[10]));
}

uint64_t get_argc(char *argv[]){
	int i =0;
	if(next_available_address == NULL){
		my_malloc_init();
	}
	for(i=0; argv[i] != NULL; i++);

	return i;
}

//TODO: Remove
void* my_malloc(uint64_t size){
	void* address = next_available_address;
	next_available_address += size;
	return address;
}

char** get_userspace_argv(uint64_t argc, char **argv){
	int i,len;
	char **uargv = (char**)my_malloc(argc * sizeof(char *));
	for(i=0; i<argc; i++){
		len = strlen(argv[i]);
		//allocate memory for the string itself
		uargv[i] = my_malloc(len+1);
		//copy the contents from userspace1 to userspace 2
		memcpy(uargv[i], argv[i], len+1);
	}
	return uargv;
}

char** get_userspace_env(char *envp[]){
	return NULL; //TODO
}

struct userspace_args  prepare_args_for_userspace(char **argv, char **envp){
	struct userspace_args ua; 
	ua.argc = get_argc(argv);
	ua.argv = get_userspace_argv(ua.argc, argv);
	ua.envp = get_userspace_env(envp);
	return ua;
}

uint64_t sys_execvpe(char *path, char *argv[], char *envp[]){
	//replace virtual adress space
	 printf("\nEXECUTING AFTER EXECVPE");
	 printf("Path %s, argv[0] %s",path, argv[0]);
	 prepare_args_for_userspace(argv, envp);
	//exec_jump_to_ring3(ua.argc, ua.argv,  ua.envp);
	return 0;
}
