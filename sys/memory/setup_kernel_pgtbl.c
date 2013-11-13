#include <sys/kstdio.h>
#include <sys/memory/pg_tbl_manager.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/page_table_helper.h>
#include <sys/memory/handle_cr2_cr3.h>
#include <sys/memory/setup_kernel_pgtbl.h>


extern struct phys_page_manager phys_page_mngr_obj;

struct str_cr3 get_default_cr3()
{
	struct str_cr3 cr3;
	cr3.reserved_low = 0x0;
	//cr3.PWT = 0x01;
	//cr3.PCD = 0x01;
	cr3.reserved_high = 0x0;
	cr3.reserved_MBZ = 0x0;

	return cr3;
}

struct page_table_helper kern_page_table_mgr;

void setup_kernel_pgtbl(void *kernmem, void *physbase, void *physfree){
	struct str_cr3 cr3 = create_kernel_pgtbl(kernmem, physbase, physfree);
	printf("New cr3: %x\n", *((uint64_t *)(&cr3)));
	set_cr3(cr3);
}


struct str_cr3 create_kernel_pgtbl(void *kernmem, 
				void *physbase, 
				void *physfree 
				)
{
	//int total_pages = (((uint64_t)physfree)-((uint64_t)physbase))/PG_SZ;
	//if((((uint64_t)physfree)-((uint64_t)physbase))%PG_SZ > 0) {
		//total_pages += 1;
	//}

	//printf("Total no of kernel pages calculated: %d\n", total_pages);	

	//void *pml4_page = get_new_pg_tbl_page();
    init_page_table_helper(&kern_page_table_mgr, &phys_page_mngr_obj);
    void *pml4_page = (void*)get_selfref_PML4(&kern_page_table_mgr);
	uint64_t paddr = (uint64_t)physbase;
	uint64_t vaddr = (uint64_t)kernmem;
	//int count = 0;
	printf("vaddr: %p, paddr: %p, physfree: %p\n", kernmem, physbase, physfree);
	while(paddr < (uint64_t)physfree) {
		//printf("vaddr: %p, paddr: %p\n", (void *)vaddr, (void *)paddr);
		update_pg_table((void *)vaddr, pml4_page, (void *)paddr);
		paddr += PG_SZ;
		vaddr += PG_SZ;
		//count++;
	}

	paddr = (uint64_t)physfree;
	vaddr = (uint64_t)physfree;
	while(paddr < (uint64_t)physfree + SIZEOF_PAGE * 300) {
		update_pg_table((void*)vaddr, pml4_page, (void*)paddr);
		paddr += PG_SZ;
		vaddr += PG_SZ;
	}

	uint64_t video_vaddr = ((uint64_t)kernmem) + ((uint64_t)physfree) + VIDEO_MEMORY_ADDRESS;
	printf("\nVIDEO MEMORY:%p",video_vaddr);
	update_pg_table((void *)(video_vaddr), pml4_page, (void *)((uint64_t)VIDEO_MEMORY_ADDRESS));
	/*update_page_table(&kern_page_table_mgr, 
			(uint64_t)pml4_page, 
			(uint64_t)VIDEO_MEMORY_ADDRESS, 
			video_vaddr, 
			PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR); */
	//printf("Total no of kernel pages added: %d\n", count);
	
	struct str_cr3 cr3 = get_default_cr3();
	cr3.p_PML4E_4Kb = ((uint64_t)pml4_page) >> 12;	//higher order 40 bits of the physical address
	printf("New PML4: %p\n", pml4_page);

	global_video_vaddr = (void *)video_vaddr;
	return cr3;
    //update_curr_page_table(&kern_page_table_mgr, (uint64_t)VIDEO_MEMORY_ADDRESS, video_vaddr, PAGE_TRANS_READ_WRITE);
}
