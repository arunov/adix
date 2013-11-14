#include <sys/memory/kmalloc.h>

extern char kernmem;

/* Page align */
#define KM_ALIGN_PAGE  0x1000ULL

/* Align */
#define KM_ALIGN(addr, align) ((addr + align - 1ULL) & (~(align - 1ULL)))

/* List of pages used for memory allocation */
LIST_HEAD(km_page_list);

/* Free list of malloc */
struct free_list {
    struct free_list *free_addr;
    uint64_t free_size;
} *head = NULL;

#define FL_SIZE sizeof(struct free_list)

/* Pages used by kmalloc */
static struct kmalloc_page_t {
    uint64_t phys_addr;
    uint64_t virt_addr;
    struct list_head km_list;
} kmalloc_page[KM_STATIC_PAGES];

/* Static pages for kmalloc */
static char km_static[KM_STATIC_SIZE] __attribute__((aligned(KM_ALIGN_PAGE)));

/**
 * Initialize kmalloc_page structure for static pages.
 */
static void init_static_pages() {
    uint64_t phys_ptr = (uint64_t)km_static;
    for(int i = 0; i < KM_STATIC_PAGES; i ++) {
        kmalloc_page[i].virt_addr = phys_ptr;
        kmalloc_page[i].phys_addr = phys_ptr - (uint64_t)&kernmem;
        list_add_tail(&(kmalloc_page[i].km_list), &km_page_list);
        phys_ptr += SIZEOF_PAGE;
    }

    head = (struct free_list*)kmalloc_page[0].virt_addr;
    head->free_addr = NULL;
    head->free_size = KM_STATIC_SIZE - FL_SIZE;

}

void* kmalloc(uint64_t bytes) {

    void *ret_addr = NULL;

    if(list_empty(&km_page_list)) {
        init_static_pages();
    }

    if(bytes == 0)
        return NULL;

    struct free_list *node = head;
    struct free_list *prev = NULL;

    while(node) {

        if(node->free_size < bytes) {
            node = node->free_addr;
        }

        ret_addr = (void*)node + FL_SIZE;

        // Check resulting free space
        if(node->free_size - bytes > FL_SIZE) {

            struct free_list *next = (struct free_list*)((void*)node + FL_SIZE
                                                                    + bytes);
            next->free_addr = node->free_addr;
            next->free_size = node->free_size - bytes - FL_SIZE;
            node->free_size = bytes;

            if(prev) {
                prev->free_addr = next;
            } else {
                head = next;
            }

        } else {

            if(prev) {
                prev->free_addr = node->free_addr;
            } else {
                head = node->free_addr;
            }

        }

        break;
    }

    // Free memory is not found
    if(ret_addr == NULL) {
        // TODO: Allocate more memory. Requires mmap()
        // NOTE: Make sure virtual memory does not go lower than current values
    }

    return ret_addr;
}

void kfree(void *ptr) {

    if(ptr == NULL)
        return;

    struct free_list *fp = (struct free_list*)(ptr - FL_SIZE);

    if(head == NULL) {
        head = fp;
        return;
    }

    struct free_list *node = head;
    struct free_list *cp, *pp;
    pp = cp = NULL;

    while(node) {
        if((uint64_t)node > (uint64_t)fp) {
            cp = node;
            break;
        }

        pp = node;

        node = node->free_addr;
    }

    // TODO: fp could be within an existing chunk
    if(fp == cp) {
        return;
    }

    if(!pp) {
        head = fp;
    }

    if(cp) {
        fp->free_addr = cp;

        // Check if fp and cp can be merged
        if((uint64_t)cp == (uint64_t)fp + FL_SIZE + fp->free_size) {
            fp->free_addr = cp->free_addr;
            fp->free_size += cp->free_size + FL_SIZE;
        }
    }

    if(pp) {
        pp->free_addr = fp;

        // Check if pp and fp can be merged
        if((uint64_t)fp == (uint64_t)pp + FL_SIZE + pp->free_size) {
            pp->free_addr = fp->free_addr;
            pp->free_size += fp->free_size + FL_SIZE;
        }
    }
}

