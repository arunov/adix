#include <defs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/kstdio.h>
#include <sys/memory/mm_struct.h>


uint64_t exec(char *filename, struct mm_struct *mm){
	return load_elf(mm, filename);
}	
