#include <sys/memory/page_table_helper.h>
#include <sys/memory/free_phys_pages.h>

void set_phys_mem_virt_map_base(uint64_t a_phys_mem_virt_map_base) {
    phys_mem_virt_map_base = a_phys_mem_virt_map_base;
}

uint64_t get_zeroed_page_trans_obj(uint64_t *phys_addr) {

    uint64_t phys = alloc_phys_pages(1);
    if(phys_addr) *phys_addr = phys;
    uint64_t page_vaddr = VIRTUAL_ADDR(phys);

    memset((void*)page_vaddr, 0, SIZEOF_PAGE_TRANS);

    return page_vaddr;
}

uint64_t get_selfref_PML4(uint64_t *phys_addr) {

    uint64_t pml4;
    uint64_t *pml4_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pml4);

    if(!pml4_vaddr) {
        if(phys_addr) *phys_addr = 0;
        return 0;
    }

    pml4_vaddr[SELF_REF_ENTRY] = PAGE_TRANS_NEXT_LEVEL_ADDR(pml4)
                                | PAGE_TRANS_PRESENT | PAGE_TRANS_READ_WRITE;

    if(phys_addr) *phys_addr = pml4;
    return (uint64_t)pml4_vaddr;
}

uint64_t get_duplicate_PML4(uint64_t src, uint64_t *phys_addr) {

    uint64_t pml4;
    uint64_t pml4_vaddr = get_zeroed_page_trans_obj(&pml4);

    if(!pml4_vaddr) {
        *phys_addr = 0;
        return 0;
    }

    memcpy((void*)pml4_vaddr, (void*)src, SIZEOF_PAGE_TRANS);
    *phys_addr = pml4;
    return pml4_vaddr;
}

int update_page_table_idmap(uint64_t pml4, uint64_t phys, uint64_t virt,
                                                        uint64_t access_perm) {

    uint64_t *pml4_vaddr = (uint64_t*) VIRTUAL_ADDR(pml4);

    uint64_t pdp, pd, pt; // physical addresses
    uint64_t *pdp_vaddr, *pd_vaddr, *pt_vaddr;

    // Check PDP present
    if(!(pml4_vaddr[INDEX_PML4E(virt)] & PAGE_TRANS_PRESENT)) {
        pdp_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pdp);
        if(!pdp_vaddr) {
            return -1;
        }

        pml4_vaddr[INDEX_PML4E(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp)
                            | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR
                                                        | PAGE_TRANS_PRESENT;
    } else {
        pdp = PAGE_TRANS_NEXT_LEVEL_ADDR(pml4_vaddr[INDEX_PML4E(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pml4_vaddr[INDEX_PML4E(virt)]);
    }

    pdp_vaddr = (uint64_t*) VIRTUAL_ADDR(pdp);

    // Check PD present
    if(!(pdp_vaddr[INDEX_PDPE(virt)] & PAGE_TRANS_PRESENT)) {
        pd_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pd);
        if(!pd_vaddr) {
            return -1;
        }

        pdp_vaddr[INDEX_PDPE(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pd)
                            | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR
                                                        | PAGE_TRANS_PRESENT;
    } else {
        pd = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp_vaddr[INDEX_PDPE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pdp_vaddr[INDEX_PDPE(virt)]);
    }

    pd_vaddr = (uint64_t*) VIRTUAL_ADDR(pd);

    // Check PT present
    if(!(pd_vaddr[INDEX_PDE(virt)] & PAGE_TRANS_PRESENT)) {
        pt_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pt);
        if(!pt_vaddr) {
            return -1;
        }

        pd_vaddr[INDEX_PDE(virt)] = PAGE_TRANS_NEXT_LEVEL_ADDR(pt)
                            | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR
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
    return 0;
}

int update_curr_page_table(uint64_t phys, uint64_t virt,
                                                        uint64_t access_perm) {

    // Design notes
    //  - This function uses self referencing trick to access page translation
    //  objects
    //  - Do NOT use phys_mem_virt_map_base directly or indirectly
    //  (VIRTUAL_ADDR, functions)

    uint64_t pdp, pd, pt; // physical addresses
    uint64_t *pml4e_vaddr, *pdpe_vaddr, *pde_vaddr, *pte_vaddr;

    pml4e_vaddr = (uint64_t*)SELF_REF_PML4E(virt);

    // Check PDP present
    if(!((*pml4e_vaddr) & PAGE_TRANS_PRESENT)) {
        pdpe_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pdp);
        if(!pdpe_vaddr) {
            return -1;
        }

        *pml4e_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp) | PAGE_TRANS_PRESENT
                        | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR;
    }

    pdpe_vaddr = (uint64_t*)SELF_REF_PDPE(virt);

    // Check PD present
    if(!((*pdpe_vaddr) & PAGE_TRANS_PRESENT)) {
        pde_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pd);
        if(!pde_vaddr) {
            return -1;
        }

        *pdpe_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pd) | PAGE_TRANS_PRESENT
                        | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR;
    }

    pde_vaddr = (uint64_t*)SELF_REF_PDE(virt);

    // Check PT present
    if(!((*pde_vaddr) & PAGE_TRANS_PRESENT)) {
        pte_vaddr = (uint64_t*)get_zeroed_page_trans_obj(&pt);
        if(!pte_vaddr) {
            return -1;
        }

        *pde_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(pt) | PAGE_TRANS_PRESENT
                        | PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR;
    }

    pte_vaddr = (uint64_t*)SELF_REF_PTE(virt);
    if(!((*pte_vaddr) & PAGE_TRANS_PRESENT)) {
        // TODO: Replace with logger
        //printf("Mapping again!");
    }

    *pte_vaddr = PAGE_TRANS_NEXT_LEVEL_ADDR(phys) | PAGE_TRANS_PRESENT
                                    | PAGE_TRANS_NON_ADDR_FIELDS(access_perm);
    return 0;
}

uint64_t virt2phys_selfref(uint64_t virt) {

    // Design notes
    //  - This function uses self referencing trick to access page translation
    //  objects
    //  - Do NOT use this->phys_mem_virt_map_base directly or indirectly
    //  (VIRTUAL_ADDR, functions)

    uint64_t *pml4e_vaddr, *pdpe_vaddr, *pde_vaddr, *pte_vaddr;

    pml4e_vaddr = (uint64_t*)SELF_REF_PML4E(virt);

    // Check PDP present
    if(!((*pml4e_vaddr) & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    pdpe_vaddr = (uint64_t*)SELF_REF_PDPE(virt);

    // Check PD present
    if(!((*pdpe_vaddr) & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    pde_vaddr = (uint64_t*)SELF_REF_PDE(virt);

    // Check PT present
    if(!((*pde_vaddr) & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    // Check page present
    pte_vaddr = (uint64_t*)SELF_REF_PTE(virt);
    if(!((*pte_vaddr) & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    return (PAGE_TRANS_NEXT_LEVEL_ADDR(*pte_vaddr)
                                        | PAGE_TRANS_ADDR_SIGN_EXT(*pte_vaddr));

}

uint64_t virt2phys_idmap(uint64_t pml4, uint64_t virt) {

    uint64_t *pml4_vaddr = (uint64_t*) VIRTUAL_ADDR(pml4);

    uint64_t pdp, pd, pt; // physical addresses
    uint64_t *pdp_vaddr, *pd_vaddr, *pt_vaddr;

    // Check PDP present
    if(!(pml4_vaddr[INDEX_PML4E(virt)] & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    pdp = PAGE_TRANS_NEXT_LEVEL_ADDR(pml4_vaddr[INDEX_PML4E(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pml4_vaddr[INDEX_PML4E(virt)]);

    pdp_vaddr = (uint64_t*) VIRTUAL_ADDR(pdp);

    // Check PD present
    if(!(pdp_vaddr[INDEX_PDPE(virt)] & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    pd = PAGE_TRANS_NEXT_LEVEL_ADDR(pdp_vaddr[INDEX_PDPE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pdp_vaddr[INDEX_PDPE(virt)]);

    pd_vaddr = (uint64_t*) VIRTUAL_ADDR(pd);

    // Check PT present
    if(!(pd_vaddr[INDEX_PDE(virt)] & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    pt = PAGE_TRANS_NEXT_LEVEL_ADDR(pd_vaddr[INDEX_PDE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pd_vaddr[INDEX_PDE(virt)]);

    pt_vaddr = (uint64_t*) VIRTUAL_ADDR(pt);

    // Check physical page present
    if(!(pt_vaddr[INDEX_PTE(virt)] & PAGE_TRANS_PRESENT)) {
        return (uint64_t)(-1);
    }

    return PAGE_TRANS_NEXT_LEVEL_ADDR(pt_vaddr[INDEX_PTE(virt)])
                    | PAGE_TRANS_ADDR_SIGN_EXT(pt_vaddr[INDEX_PTE(virt)]);
}

