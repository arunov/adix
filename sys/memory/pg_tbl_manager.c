#include <sys/kstdio.h>
#include <sys/memory/page_trans_4kb.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/pg_tbl_manager.h>

extern struct phys_page_manager phys_page_mngr_obj;

struct page_trans_4Kb get_default_kernel_PML4E_PDPE_PDE()
{
        struct page_trans_4Kb entry;
        entry.P = 0x1;
        entry.RW = 0x1;
        entry.US = 0x1;
        //entry.PWT = 0x1;
        //entry.PCD = 0x1;
        entry.bits6_8 = 0x0;
        entry.available = 0x7ff;  //To maintain canonical form
        entry.NX = 0x0;

        return entry;
}

struct page_trans_4Kb get_default_kernel_PTE()
{
        struct page_trans_4Kb entry;
        entry.P = 0x1;
        entry.RW = 0x1;
        entry.US = 0x1;
        //entry.PWT = 0x1;
        //entry.PCD = 0x1;
        entry.bits6_8 = 0x4;      //G=1, PAT=0, D=0
        entry.available = 0x7ff;  //To maintain canonical form
        entry.NX = 0x0;

        return entry;
}

void *get_new_pg_tbl_page()
{
	void *page = get_free_phys_page(&phys_page_mngr_obj);
	//printf("Physical Page: %p\n", page);
	if( (((uint64_t)page) & PG_SZ_MASK) == (uint64_t)page){
		//printf("Correct page for PG_TBL\n");
	}
	else{
		//printf("Wrong page for PG_TBL!!!\n"); 
	}
	
	struct page_trans_4Kb *pml4e = (struct page_trans_4Kb *)page;
	int i=0;
	for(i=0; i<512; i++){
		pml4e[i].P = 0x0;
	}

	return page;
}

void update_pg_table(void *vaddr, void *pml4_pg, void *new_page)
{
	uint64_t pte_offset = (((uint64_t)vaddr) >> 12) & ENTRY_OFFSET;		//bits 12-20
	uint64_t pde_offset = (((uint64_t)vaddr) >> 21) & ENTRY_OFFSET;		//bits 21-29
	uint64_t pdpe_offset = (((uint64_t)vaddr) >> 30) & ENTRY_OFFSET;	//bits 30-38
	uint64_t pml4e_offset = (((uint64_t)vaddr) >> 39) & ENTRY_OFFSET;	//bits 39-47

	//printf("PML4E: %x, PDP: %x, PD: %x, PT: %x\n", pml4e_offset, pdpe_offset, pde_offset, pte_offset);

	struct page_trans_4Kb *pml4e = (struct page_trans_4Kb *)pml4_pg;
	if(pml4e[pml4e_offset].P == 0x0) {
		void *page = get_new_pg_tbl_page();
		//printf("New Page: %p\n", page);
		struct page_trans_4Kb new_pml4e = get_default_kernel_PML4E_PDPE_PDE();
		new_pml4e.P = 0x01;
		new_pml4e.p_PDPE_4Kb = ((uint64_t)page) >> 12;	//higher order 40 bits of the physical address
		pml4e[pml4e_offset] = new_pml4e;	
	}

	//printf("PDPE Page: %p\n", (struct page_trans_4Kb *)((uint64_t)(pml4e[pml4e_offset].p_PDPE_4Kb << 12)));
	struct page_trans_4Kb *pdpe = (struct page_trans_4Kb *)((uint64_t)(pml4e[pml4e_offset].p_PDPE_4Kb << 12));
	if(pdpe[pdpe_offset].P == 0x0) {
		void *page = get_new_pg_tbl_page();
        	struct page_trans_4Kb new_pdpe = get_default_kernel_PML4E_PDPE_PDE();
                new_pdpe.P = 0x01;
                new_pdpe.p_PDPE_4Kb = ((uint64_t)page) >> 12;	//higher order 40 bits of the physical address  
                pdpe[pdpe_offset] = new_pdpe;
	}
                	
	struct page_trans_4Kb *pde = (struct page_trans_4Kb *)((uint64_t)(pdpe[pdpe_offset].p_PDPE_4Kb << 12));
      	if(pde[pde_offset].P == 0x0) {
               	void *page = get_new_pg_tbl_page();
              	struct page_trans_4Kb new_pde = get_default_kernel_PML4E_PDPE_PDE();
                new_pde.P = 0x01;
       		new_pde.p_PDPE_4Kb = ((uint64_t)page) >> 12;	//higher order 40 bits of the physical address
               	pde[pde_offset] = new_pde;
       	}
         
	//printf("New Page to be added: %p\n", new_page);       		
	struct page_trans_4Kb *pte = (struct page_trans_4Kb *)((uint64_t)(pde[pde_offset].p_PDPE_4Kb << 12));
	if(pte[pte_offset].P == 0x0) {
      		struct page_trans_4Kb new_pte = get_default_kernel_PTE();
      		new_pte.P = 0x01;
       		new_pte.p_PDPE_4Kb = ((uint64_t)new_page) >> 12;	//higher order 40 bits of the physical address
       		pte[pte_offset] = new_pte;
	}
	else {
		printf("Page already mapped!!!\n");
	}
}














