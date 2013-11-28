#ifndef _EXEC_H
#define _EXEC_H
#include <sys/memory/mm_struct.h>

int exec(char *filename, struct mm_struct *mm);

#endif
