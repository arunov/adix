#include<sys/memory/page_table_helper.h>
#include<sys/memory/phys_page_manager.h>
#include<sys/memory/virtual_page_manager.h>

//TODO: should be replaced with vma address assignement logic
uint64_t getVirtualAddress(uint64_t phys){
	return phys;//offsetting kernmem-physbase page faults
}

void* getFreeVirtualPage(){
	uint64_t phys = (uint64_t)get_free_phys_page(getPhysPageManager());
	uint64_t virt = getVirtualAddress(phys);
	printf("Getting a free virtual page at address:%x\n",virt); //TODO:DEBUG
	update_curr_page_table(getPageTableHelper(), 
			       phys, 
			       virt, 
			    PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
	return (void*)virt;			
}
