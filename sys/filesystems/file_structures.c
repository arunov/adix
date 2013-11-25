#include <sys/filesystems/file_structures.h>
#include <defs.h>
#include <sys/memory/kmalloc.h>
struct process_files_table* get_new_process_files_table(
				struct posix_header_ustar *header,
				uint64_t offset,
				struct operation_pointers *op_pointers
				){
	
	struct process_files_table* pft = (struct process_files_table*) kmalloc(
						sizeof(struct process_files_table));
	pft->header = header;
	pft->offset = offset;
	pft->op_pointers = op_pointers;
	return pft;
}
