#include<sys/memory/virtual_page_manager.h>
#include<sys/memory/free_phys_pages.h>
#include<sys/memory/page_table_helper.h>

void* getFreeVirtualPage(){
    uint64_t virt = v_alloc_pages(1, PAGE_TRANS_READ_WRITE
                                                | PAGE_TRANS_USER_SUPERVISOR);
	printf("Getting a free virtual page at address:%x\n",virt); //TODO:DEBUG
	return (void*)virt;			
}

