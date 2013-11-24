#include <sys/memory/page_fault_handler.h>
#include <sys/memory/handle_cr.h>
#include <sys/scheduler/pcb.h>
#include <sys/scheduler/scheduler.h>
#include <sys/list.h>
#include <sys/memory/mm_struct.h>
#include <sys/memory/page_constants.h>
#include <sys/kstdio.h>
#include <sys/memory/free_phys_pages.h>

#define COW_ERROR   -1
#define COW_SUCCESS  0
#define NOT_COW      1

static int check_page_prot(struct vm_area_struct *node, uint64_t fault_addr) {

    uint64_t perm;
    virt2phys_selfref(fault_addr, &perm);

    if(node->vm_page_prot == perm) {
        return -1;
    }

    return update_curr_page_table(0, fault_addr & PAGE_ALIGN,
                                    PAGE_TRANS_PRESENT | node->vm_page_prot);
}

static void segfault(uint64_t fault_addr) {

    printf("\nSometimes life hits you in the head with a brick. Don't lose "
                                        "faith. Seg fault at %p\n", fault_addr);
    //sys_exit(1);
}

static int check_priv(struct vm_area_struct *node, uint64_t fault_addr,
                                                            uint64_t err_code) {
    if((err_code & PF_ERROR_CODE_U) &&
                        !(node->vm_page_prot & PAGE_TRANS_USER_SUPERVISOR)) {
        segfault(fault_addr);
        return -1;
    }

    return 0;
}

static int cow(struct vm_area_struct *node, uint64_t fault_addr) {

    return 1;
}

/**
 * Handle #PF when page is not present
 * @param node memory region corresponding to fault_addr
 * @param fault_addr virtual address that caused #PF
 */
static int page_not_present(struct vm_area_struct *node, uint64_t fault_addr) {

    if(!node) {
        segfault(fault_addr);
        return -1;
    }

    // Get new physical page
    uint64_t phys = alloc_phys_pages(1);

    if(!phys) {
        printf("Cannot allocate physical page :(. Exiting!\n");
        sys_exit(1);
        return -1;
    }

    // Add to page table
    update_curr_page_table(phys, fault_addr & PAGE_ALIGN, node->vm_page_prot);

    return 0;
}

/**
 * Handle #PF due to write access
 * @param node memory region corresponding to fault_addr
 * @param fault_addr virtual address that caused #PF
 */
static int page_present_write(struct vm_area_struct *node,
                                    uint64_t fault_addr, uint64_t err_code) {

    if(node->vm_page_prot & PAGE_TRANS_READ_WRITE) {

        // Check COW
        int cow_status = cow(node, fault_addr);

        if(cow_status != NOT_COW) {
            return cow_status;
        }

        if(-1 == check_priv(node, fault_addr, err_code)) {
            return -1;
        }

        if(0 == check_page_prot(node, fault_addr)) {
            return 0;
        }
    }

    segfault(fault_addr);
    return -1;
}

/**
 * Handle #PF due to read access
 * @param node memory region corresponding to fault_addr
 * @param fault_addr virtual address that caused #PF
 */
static int page_present_read(struct vm_area_struct *node,
                                    uint64_t fault_addr, uint64_t err_code) {

    if(-1 == check_priv(node, fault_addr, err_code)) {
        return -1;
    }

    if(0 == check_page_prot(node, fault_addr)) {
        return 0;
    }

    segfault(fault_addr);
    return -1;
}

int page_fault_handler(uint64_t err_code) {

    // Get virtual memory address that caused #PF
    uint64_t fault_addr = (uint64_t) get_cr2();

    // Get memory area
    struct list_head *vma_list_head;

//    if(fault_addr >= get_kernel_mm()->start_kernel) {
        vma_list_head = &(get_kernel_mm()->mmap);
//    } else {
//        vma_list_head = &(getCurrentTask()->mm->mmap);
//    }

    // Get memory region
    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, vma_list_head, vm_list) {
        if(fault_addr >= node->vm_start && fault_addr < node->vm_end) {
            break;
        }
    }

    if(&(node->vm_list) == vma_list_head) {
        node = NULL;
    }

    // Is page present?
    if(err_code & PF_ERROR_CODE_P) {

        // Is #PF due to write access?
        if(err_code & PF_ERROR_CODE_W)
            return page_present_write(node, fault_addr, err_code);
        else
            return page_present_read(node, fault_addr, err_code);

    } else {

        return page_not_present(node, fault_addr);
    }

}

