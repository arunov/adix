#include <sys/kstdio.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/page_table_helper.h>
#include <sys/memory/handle_cr.h>
#include <sys/memory/setup_kernel_pgtbl.h>
#include <sys/memory/mm_struct.h>
#include <sys/memory/kmalloc.h>
#include <sys/memory/phys_page_manager2.h>

extern struct phys_page_manager phys_page_mngr_obj;

void setup_kernel_pgtbl(void *kernmem, void *physbase, void *physfree){
	struct str_cr3 cr3 = create_kernel_pgtbl(kernmem, physbase, physfree);
	printf("New cr3: %x\n", *((uint64_t *)(&cr3)));
	set_cr3(cr3);
    uint64_t prot;
    printf("kernmem %p has prot %p and virtual addr %p\n", kernmem, prot, virt2phys_selfref((uint64_t)kernmem, &prot));
    kmDeviceMemorySetUpDone();
}


struct str_cr3 create_kernel_pgtbl(void *kernmem, 
				void *physbase, 
				void *physfree 
				)
{
    uint64_t pml4_page;
    get_selfref_PML4(&pml4_page);
	uint64_t paddr = (uint64_t)physbase;
	uint64_t vaddr = (uint64_t)kernmem;

	while(paddr < (uint64_t)physfree) {
	    update_page_table_idmap( 
			pml4_page, 
			paddr, 
			vaddr, 
			PAGE_TRANS_READ_WRITE);
		paddr += PG_SZ;
		vaddr += PG_SZ;
	}

    phys_mem_offset_map(pml4_page, get_kernel_mm()->start_phys_mem);

	uint64_t video_vaddr = get_kernel_mm()->start_vdo_buff;
	printf("\nVIDEO MEMORY:%p",video_vaddr);
	update_page_table_idmap(
			pml4_page, 
			(uint64_t)VIDEO_MEMORY_ADDRESS, 
			video_vaddr, 
			PAGE_TRANS_READ_WRITE);
	
	struct str_cr3 cr3 = get_default_cr3();
	cr3.p_PML4E_4Kb = pml4_page >> 12;	//higher order 40 bits of the physical address
	printf("New PML4: %p\n", pml4_page);

    char *x = kmalloc(10*4096);
    memcpy(x, "I am x :)", 10);
    printf("*x: %s x: %p\n", x, x);
    char *y = kmalloc(100);
    memcpy(y, "I am y :)", 10);
    printf("*y: %s y: %p\n", y, y);
    kfree(x);
    kfree(y);
	global_video_vaddr = (void *)video_vaddr;
	return cr3;

}

