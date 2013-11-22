#ifndef _PHYS_PAGE_MANAGER2_H
#define _PHYS_PAGE_MANAGER2_h

#include <defs.h>
#include <sys/memory/page_constants.h>
#include <sys/list.h>

#define PAGE_COW 0x1UL

/**
 * Physical page descriptor
 */
struct phys_page_t {
    /* Reference count - Number of references to this page */
    uint32_t refcount;

    /*
     * PAGE_COW - Page is in Copy On Write mode
     */
    uint32_t flags;
};

/**
 * Chunk is a contiguos physical memory addresses
 */
struct chunk_t {
    /* Start (physical) address of chunk */
    uint64_t phys_addr_start;

    /* End (physical) address of chunk */
    uint64_t phys_addr_end;

    /* Each page in chunk has a phys_page_t. Pointers to phys_page_t objects for
    pages of a chunk are kept in an array (table). phys_page_pointer is pointer
    to the table. */
    struct phys_page_t **phys_page_pointer;

    /* List of chunks */
    struct list_head chunk_list;
};

/**
 * Create a new chunk of physical memory. Note: phys_page_t objects are not
 * created! Use scan_chunk or scan_all_chunks for creating phys_page_t objects.
 * @param start starting address of chunk
 * @param end   ending address of chunk
 * @return      pointer to new chunk or NULL
 */
struct chunk_t* new_chunk(uint64_t start, uint64_t end);

/**
 * Create zero initialized phys_page_t objects for pages in chunk
 * @param chunk pointer to chunk
 * @return      OK or ERROR
 */
int scan_chunk(struct chunk_t *chunk);

/**
 * Create zero initialized phys_page_t objects for pages in all chunks
 * @return OK or ERROR
 */
int scan_all_chunks();

/**
 * Increment refcount of page
 * @param addr physical address of page for which refcount is to be incremented
 * @return     refcount or ERROR
 */
int inc_ref_count_page(uint64_t addr);

/**
 * Decrement refcount of page
 * @param addr physical address of page for which refcount is to be decremented
 * @return     refcount or ERROR
 */
int dec_ref_count_page(uint64_t addr);

/**
 * Increment refcounts of several pages
 * @param start_addr starting address
 * @param end_addr   ending address
 * @return           OK or ERROR
 */
int inc_ref_count_pages(uint64_t start_addr, uint64_t end_addr);

#endif

