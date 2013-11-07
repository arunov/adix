#include <sys/memory/page_table_helper.h>

static struct page_table_helper *g_pg_table_helper;
void init_page_table_helper(struct page_table_helper *this,
                                    struct phys_page_manager *a_phys_page_mgr) {

    this->phys_mgr = a_phys_page_mgr;
    this->phys_mem_virt_map_base = 0;
    g_pg_table_helper = this;
}

void set_phys_mem_virt_map_base(struct page_table_helper *this,
                                            uint64_t a_phys_mem_virt_map_base) {

    this->phys_mem_virt_map_base = a_phys_mem_virt_map_base;
}

uint64_t get_zeroed_page_trans_obj(struct page_table_helper *this) {

    uint64_t page = (uint64_t)get_free_phys_page(this->phys_mgr);
    uint64_t *page_vaddr = (uint64_t*) VIRTUAL_ADDR(page);

    memset((void*)page_vaddr, 0, SIZEOF_PAGE_TRANS);

    return page;
}

uint64_t get_selfref_PML4(struct page_table_helper *this) {

    uint64_t pml4 = get_zeroed_page_trans_obj(this);
    uint64_t *pml4_vaddr = (uint64_t*) VIRTUAL_ADDR(pml4);

    pml4_vaddr[SELF_REF_ENTRY] = PAGE_TRANS_NEXT_LEVEL_ADDR(pml4)
                                | PAGE_TRANS_PRESENT | PAGE_TRANS_READ_WRITE;

    return pml4;
}

uint64_t get_duplicate_PML4(struct page_table_helper *this, uint64_t src) {

    uint64_t pml4 = get_zeroed_page_trans_obj(this);
    uint64_t *pml4_vaddr = (uint64_t*) VIRTUAL_ADDR(pml4);
    uint64_t *src_vaddr = (uint64_t*) VIRTUAL_ADDR(src);

    memcpy((void*)pml4_vaddr, (void*)src_vaddr, SIZEOF_PAGE_TRANS);

    return pml4;
}

void update_page_table(struct page_table_helper *this, uint64_t pml4,
                        uint64_t phys, uint64_t virt, uint64_t access_perm) {

    uint64_t *pml4_vaddr = (uint64_t*) VIRTUAL_ADDR(pml4);

    uint64_t pdp, pd, pt; // physical addresses
    uint64_t *pdp_vaddr, *pd_vaddr, *pt_vaddr;

    // Check PDP present
    if(!(pml4_vaddr[INDEX_PML4E(virt)] & PAGE_TRANS_PRESENT)) {
        pdp = get_zeroed_page_trans_obj(this);
        pml4_vaddr[INDEX_PML4E(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp)
                                                        | PAGE_TRANS_PRESENT;
    } else {
        pdp = PAGE_TRANS_NEXT_LEVEL_ADDR(pml4_vaddr[INDEX_PML4E(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pml4_vaddr[INDEX_PML4E(virt)]);
    }

    pdp_vaddr = (uint64_t*) VIRTUAL_ADDR(pdp);

    // Check PD present
    if(!(pdp_vaddr[INDEX_PDPE(virt)] & PAGE_TRANS_PRESENT)) {
        pd = get_zeroed_page_trans_obj(this);
        pdp_vaddr[INDEX_PDPE(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pd)
                                                        | PAGE_TRANS_PRESENT;
    } else {
        pd = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp_vaddr[INDEX_PDPE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pdp_vaddr[INDEX_PDPE(virt)]);
    }

    pd_vaddr = (uint64_t*) VIRTUAL_ADDR(pd);

    // Check PT present
    if(!(pd_vaddr[INDEX_PDE(virt)] & PAGE_TRANS_PRESENT)) {
        pt = get_zeroed_page_trans_obj(this);
        pd_vaddr[INDEX_PDE(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pt)
                                                        | PAGE_TRANS_PRESENT;
    } else {
        pt = PAGE_TRANS_NEXT_LEVEL_ADDR(pd_vaddr[INDEX_PDE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pd_vaddr[INDEX_PDE(virt)]);
    }

    pt_vaddr = (uint64_t*) VIRTUAL_ADDR(pt);

    // Check physical page present
    if(pt_vaddr[INDEX_PTE(virt)] & PAGE_TRANS_PRESENT) {
        // TODO: Replace with logger
        //printf("Mapping again!");
    }

    pt_vaddr[INDEX_PTE(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(phys)
                | PAGE_TRANS_PRESENT | PAGE_TRANS_NON_ADDR_FIELDS(access_perm);

}

void update_curr_page_table(struct page_table_helper *this, uint64_t phys,
                                        uint64_t virt, uint64_t access_perm) {

    // Design notes
    //  - This function uses self referencing trick to access page translation
    //  objects
    //  - Do NOT use this->phys_mem_virt_map_base directly or indirectly
    //  (VIRTUAL_ADDR, functions)

    uint64_t pdp, pd, pt; // physical addresses
    uint64_t *pml4e_vaddr, *pdpe_vaddr, *pde_vaddr, *pte_vaddr;


    pml4e_vaddr = (uint64_t*)SELF_REF_PML4E(virt);

    // Check PDP present
    if(!((*pml4e_vaddr) & PAGE_TRANS_PRESENT)) {
        pdp = (uint64_t)get_free_phys_page(this->phys_mgr);
        *pml4e_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp) | PAGE_TRANS_PRESENT;
        memset((void*)(SELF_REF_PDP(virt)), 0, SIZEOF_PAGE_TRANS); 
    }

    pdpe_vaddr = (uint64_t*)SELF_REF_PDPE(virt);

    // Check PD present
    if(!((*pdpe_vaddr) & PAGE_TRANS_PRESENT)) {
        pd = (uint64_t)get_free_phys_page(this->phys_mgr);
        *pdpe_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pd) | PAGE_TRANS_PRESENT;
        memset((void*)(SELF_REF_PD(virt)), 0, SIZEOF_PAGE_TRANS);
    }

    pde_vaddr = (uint64_t*)SELF_REF_PDE(virt);

    // Check PT present
    if(!((*pde_vaddr) & PAGE_TRANS_PRESENT)) {
        pt = (uint64_t)get_free_phys_page(this->phys_mgr);
        *pde_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pt) | PAGE_TRANS_PRESENT;
        memset((void*)(SELF_REF_PT(virt)), 0, SIZEOF_PAGE_TRANS);
    }

    pte_vaddr = (uint64_t*)SELF_REF_PTE(virt);
    if(!((*pte_vaddr) & PAGE_TRANS_PRESENT)) {
        // TODO: Replace with logger
        //printf("Mapping again!");
    }

    *pte_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(phys) | PAGE_TRANS_PRESENT
                                    | PAGE_TRANS_NON_ADDR_FIELDS(access_perm);

}

struct page_table_helper *getPageTableHelper() {
	return g_pg_table_helper;
}
