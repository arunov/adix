#include <sys/memory/kmalloc.h>

extern char kernmem;

/* Align */
#define KM_ALIGN(addr, align) ((addr + align - 1ULL) & (~(align - 1ULL)))

/* List of pages used for memory allocation */
LIST_HEAD(km_page_list);

/* Next allocatable virtual address */
uint64_t bump_pos;

/* Available memory */
uint64_t avail_mem;

static struct kmalloc_page_t {
    uint64_t phys_addr;
    uint64_t virt_addr;
    struct list_head km_list;
} kmalloc_page[KM_STATIC_PAGES];

static char km_static[KM_STATIC_SIZE];

static void init_static_pages() {
    uint64_t phys_ptr = (uint64_t)km_static;
    for(int i = 0; i < KM_STATIC_PAGES; i ++) {
        kmalloc_page[i].virt_addr = phys_ptr;
        kmalloc_page[i].phys_addr = phys_ptr - (uint64_t)&kernmem;
        list_add_tail(&(kmalloc_page[i].km_list), &km_page_list);
        phys_ptr += SIZEOF_PAGE;
    }

    bump_pos = kmalloc_page[0].virt_addr;
    avail_mem = KM_STATIC_SIZE;
}

void* kmalloc_align(uint64_t bytes, uint64_t align) {

    uint64_t ret_addr = NULL;

    if(list_empty(&km_page_list)) {
        init_static_pages();
    }

    // Align
    ret_addr = KM_ALIGN(bump_pos, align);
    uint64_t avail_mem_align = avail_mem - (ret_addr - bump_pos);

    // TODO: Logger
    printf("Before alloc> avail_mem: %p, bump_pos: %p\n", avail_mem, bump_pos);

    if(bytes <= avail_mem_align) {

        avail_mem = avail_mem_align - bytes;
        bump_pos = ret_addr + bytes;

        // TODO: Logger
        printf("After alloc> avail_mem: %p, bump_pos: %p\n", avail_mem,
                                                                    bump_pos);
    }

    // TODO: Allocate more memory. Requires mmap()
    return (void*) ret_addr;

}

void kfree(void *ptr) {

    // TODO: Reclaim freed memory!

}

