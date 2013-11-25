#ifndef _FREE_PHYS_PAGES_H
#define _FREE_PHYS_PAGES_H

#include <defs.h>
#include <sys/list.h>
#include <sys/memory/page_table_helper.h>

/**
 * Free list of physical pages
 */
struct phys_free_list_t {
    /* Starting physical address */
    uint64_t addr;

    /* Number of pages */
    uint32_t count;

    /* List */
    struct list_head phys_free_list;
};

/**
 * Scans through physical pages organized as chunk lists.
 * @return OK or ERROR
 */
int init_free_phys_page_manager();

/**
 * Allocate physical pages. Unless contiguous physical memory is not required,
 * use loops where each request is for 1 page. This mitigates alloc failure.
 * @param n number of physical pages
 * @return  physical address or NULL on error
 */
uint64_t alloc_phys_pages(uint32_t n);

/**
 * Free physcal page. Each page has to be individually freed!
 * @param addr physical address of page to be freed
 * @return     OK or ERROR
 */
int free_phys_page(uint64_t addr);

/**
 * Allocate physical page and do much more.
 * NOTE: This function uses self ref page tables and hence cannot be used until
 *       self ref page tables are set up.
 * @param phys_addr pointer to location where physical address of page is to be
 *                  returned
 * @param prot      page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                  PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                  (@see include/sys/memory/page_table_helper.h).
 * @return          virtual address of page (in kernel space), NULL on error
 */
uint64_t v_alloc_page_get_phys(uint64_t *phys_addr, uint64_t prot);

/**
 * Allocate physical page and do much more.
 * NOTE: This function uses self ref page tables and hence cannot be used until
 *       self ref page tables are set up.
 * @param phys_addr pointer to location where physical address of page is to be
 *                  returned
 * @param prot      page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                  PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                  (@see include/sys/memory/page_table_helper.h).
 * @param virt      virtual address of page (no guarantee)
 * @return          virtual address of page, NULL on error
 */
uint64_t v_alloc_page_get_phys_at_virt(uint64_t *phys_addr, uint64_t prot,
                                                                uint64_t virt);
/**
 * Allocate pages.
 * @param n    number of pages
 * @param prot page protection. E.g. PAGE_TRANS_READ_WRITE |
 *             PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *             (@see include/sys/memory/page_table_helper.h).
 * @return     virtual address of first page (in kernel space), NULL on error
 */
uint64_t v_alloc_pages(uint32_t n, uint64_t prot);

/**
 * Allocate pages.
 * @param n    number of pages
 * @param prot page protection. E.g. PAGE_TRANS_READ_WRITE |
 *             PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *             (@see include/sys/memory/page_table_helper.h).
 * @param virt virtual address of page (no guarantee)
 * @return     virtual address of first page, NULL on error
 */
uint64_t v_alloc_pages_at_virt(uint32_t n, uint64_t prot, uint64_t virt);

/**
 * Free page
 * @param virt virtual address of page to be freed
 * @return     OK or ERROR
 */
int v_free_page(uint64_t virt);

#endif

