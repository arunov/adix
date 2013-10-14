#ifndef __PG_TBL_MANAGER_H
#define __PG_TBL_MANAGER_H

#define ENTRY_OFFSET 0x1ffUL

struct page_trans_4Kb get_default_kernel_PML4E_PDPE_PDE();
struct page_trans_4Kb get_default_kernel_PTE();

void *get_new_pg_tbl_page();
void update_pg_table(void *vaddr, void *pml4_pg, void *new_page);

#endif
