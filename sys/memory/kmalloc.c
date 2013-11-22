#include <sys/memory/kmalloc.h>
#include <sys/memory/free_phys_pages.h>

extern char kernmem;

/* Page align */
#define KM_ALIGN_PAGE  0x1000ULL

/* Align */
#define KM_ALIGN(addr, align) ((addr + align - 1ULL) & (~(align - 1ULL)))

/* Flag indicating device memory is set up */
static int deviceMemorySetUp = 0;

/* Free list of malloc */
struct free_list {
    struct free_list *free_addr;
    uint64_t free_size;
} *head = NULL;

#define FL_SIZE sizeof(struct free_list)

/* Static pages for kmalloc */
static char km_static[KM_STATIC_SIZE] __attribute__((aligned(KM_ALIGN_PAGE)));

/* Last virtual address of allocatable pages */
static uint64_t last_km_addr = 0;

/* Init static pages */
static char init_static_pages_done = 0;

/**
 * Initialize kmalloc_page structure for static pages.
 */
static void init_static_pages() {

    head = (struct free_list*)km_static;
    head->free_addr = NULL;
    head->free_size = KM_STATIC_SIZE - FL_SIZE;
    last_km_addr = (uint64_t)km_static + KM_STATIC_SIZE;
    init_static_pages_done = 1;
}

void* kmalloc(uint64_t bytes) {

    void *ret_addr = NULL;

    if(!init_static_pages_done) {
        init_static_pages();
    }

    if(bytes == 0)
        return NULL;

    struct free_list *node = head;
    struct free_list *prev = NULL;

    while(node) {

        if(node->free_size < bytes) {
            prev = node;
            node = node->free_addr;
            continue;
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
    if(ret_addr == NULL && deviceMemorySetUp) {
        // NOTE: Make sure virtual memory does not go lower than current values
        /*uint64_t virt_addr_start, virt_addr;

        virt_addr_start = virt_addr = mmap(&(get_kernel_mm()->mmap),
                            (uint64_t)&kernmem, KM_PAGES_ADD * SIZEOF_PAGE,
                            PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR,
                            MAP_ANONYMOUS, 0, 0);

        for(int i = 0; i < KM_PAGES_ADD; i ++) {
            // Get free physical page
            uint64_t phys = (uint64_t) get_free_phys_page(getPhysPageManager());
            // TODO: Check return value of phys

            // Add to page table
            update_curr_page_table(phys, virt_addr, PAGE_TRANS_READ_WRITE
                                                | PAGE_TRANS_USER_SUPERVISOR);

            virt_addr += SIZEOF_PAGE;
        }*/

        uint64_t virt_addr_start = v_alloc_pages_at_virt(KM_PAGES_ADD,
                                        PAGE_TRANS_READ_WRITE, last_km_addr);
        last_km_addr = virt_addr_start + KM_PAGES_ADD * SIZEOF_PAGE;
        if(virt_addr_start == 0) {
            return 0;
        }

        if(!head) {
            head = (struct free_list*)virt_addr_start;
            head->free_addr = NULL;
            head->free_size = KM_PAGES_ADD * SIZEOF_PAGE;
        } else {

            prev = head;

            while(prev->free_addr != NULL) prev = prev->free_addr;

            if(virt_addr_start == (uint64_t)prev + FL_SIZE + prev->free_size) {
                prev->free_size += KM_PAGES_ADD * SIZEOF_PAGE;
            } else {
                node = (struct free_list*)virt_addr_start;
                node->free_addr = NULL;
                node->free_size = KM_PAGES_ADD * SIZEOF_PAGE;
                prev->free_addr = node;
           }
        }

        return kmalloc(bytes);

    } else if (ret_addr == NULL){
        // KERNEL PANIC
        // TODO: Logger
        printf("KERNEL PANIC!!!");
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

void kmDeviceMemorySetUpDone() {
    deviceMemorySetUp = 1;
}

