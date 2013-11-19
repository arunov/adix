#include <defs.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/kstdio.h>
#include <sys/memory/mm_struct.h>


int exec(char *filename){
	struct mm_struct *mm = new_mm();
	uint64_t load_succ = load_elf(mm, filename);
	return (load_succ==-1)?-1:0;
}	
