#include <sys/memory/phys_page_manager2.h>
#include <sys/memory/kmalloc.h>

LIST_HEAD(chunk_list_head);

struct chunk_t* new_chunk(uint64_t start, uint64_t end) {

    start = (start + SIZEOF_PAGE - 1UL) & PAGE_ALIGN;
    end &= PAGE_ALIGN;

    if(start >= end) {
        return NULL;
    }

    uint64_t num_phys_pages = (end - start) >> LOG2_SIZEOF_PAGE;

    struct chunk_t *chunk = (struct chunk_t*) kmalloc(
                                            (uint64_t)sizeof(struct chunk_t));

    if(chunk == NULL) {
        return NULL;
    }

    chunk->phys_page_pointer = (struct phys_page_t**) kmalloc(
                                    sizeof(struct chunk_t*) * num_phys_pages);

    if(chunk->phys_page_pointer == NULL) {
        kfree(chunk);
        return NULL;
    }

    memset(chunk->phys_page_pointer, 0, sizeof(
                                            struct chunk_t*) * num_phys_pages);

    chunk->phys_addr_start = start;
    chunk->phys_addr_end = end;

    // Add to list
    // TODO: Keep sorted list
    list_add_tail(&chunk->chunk_list, &chunk_list_head);

    return chunk;
}

int scan_chunk(struct chunk_t *chunk) {

    if(!chunk) {
        return -1;
    }

    uint64_t num_phys_pages = (chunk->phys_addr_end - chunk->phys_addr_start)
                                                            >> LOG2_SIZEOF_PAGE;

    for(int i = 0; i < num_phys_pages; i ++) {
        chunk->phys_page_pointer[i] = (struct phys_page_t*) kmalloc(
                                                    sizeof(struct phys_page_t));

        if(chunk->phys_page_pointer[i] == NULL) {
            return -1;
        }

        memset(chunk->phys_page_pointer[i], 0, sizeof(struct phys_page_t));
    }

    return 0;
}

int scan_all_chunks() {

    struct chunk_t *node;

    list_for_each_entry(node, &chunk_list_head, chunk_list) {

        if(-1 == scan_chunk(node)) {
            return -1;
        }
    }

    return 0;
}

static struct phys_page_t* get_phys_page(uint64_t addr) {

    addr &= PAGE_ALIGN;
    struct chunk_t *node;

    list_for_each_entry(node, &chunk_list_head, chunk_list) {

        if(addr >= node->phys_addr_start && addr < node->phys_addr_end) {
            uint64_t index = (addr - node->phys_addr_start) >> LOG2_SIZEOF_PAGE;
            return ((node->phys_page_pointer)[index]);
        }
    }

    return NULL;
}

int inc_ref_count_page(uint64_t addr) {

    struct phys_page_t *node = get_phys_page(addr);

    if(node) {
        node->refcount ++;
        return (int)node->refcount;
    }

    return -1;
}

int dec_ref_count_page(uint64_t addr) {

    struct phys_page_t *node = get_phys_page(addr);

    if(node) {
        node->refcount --;
        return (int)node->refcount;
    }

    return -1;
}

int inc_ref_count_pages(uint64_t start_addr, uint64_t end_addr) {

    start_addr &= PAGE_ALIGN;
    end_addr = (end_addr + SIZEOF_PAGE - 1UL) & PAGE_ALIGN;
    uint64_t addr = start_addr;
    int ret_val = 0;

    // TODO: Do more efficiently
    for(; addr < end_addr; addr += SIZEOF_PAGE) {
        ret_val += inc_ref_count_page(addr);
    }

    return ret_val;
}

int phys_mem_offset_map(uint64_t pml4, uint64_t phys_mem_offset) {

    struct chunk_t *node;

    list_for_each_entry(node, &chunk_list_head, chunk_list) {

        uint64_t paddr = node->phys_addr_start;

        while(paddr < node->phys_addr_end) {

            // TODO: Check return value
            update_page_table_idmap(pml4, paddr, paddr + phys_mem_offset,
                                                        PAGE_TRANS_READ_WRITE);
            paddr += SIZEOF_PAGE;
        }
    }

    return 0;
}

