#ifndef _EXEC_H
#define _EXEC_H
#include <defs.h>
uint64_t sys_execvpe(char *path, char *argv[], char *envp[]);
#endif

