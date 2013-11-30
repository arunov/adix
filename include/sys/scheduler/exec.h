#ifndef _EXEC_H
#define _EXEC_H
#include <defs.h>
struct userspace_args{
	uint64_t argc;
	char **argv;
	char **envp;
};
uint64_t sys_execvpe(char *path, char *argv[], char *envp[]);
#endif

