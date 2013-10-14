#ifndef __PG_TBL_MANAGER_H
#define __PG_TBL_MANAGER_H

#define PML4_OFFSET (0xff80000000000000UL >> 16)
#define PDP_OFFSET (0xff80000000000000UL >> 25)
#define PD_OFFSET (0xff80000000000000UL >> 34)
#define PT_OFFSET (0xff80000000000000UL >> 43)

struct page_trans_4Kb get_default_kernel_PML4E_PDPE_PDE();
struct page_trans_4Kb get_default_kernel_PTE();

void *get_new_pg_tbl_page();
void update_pg_table(void *vaddr, void *pml4_pg, void *new_page);

#endif
