#include <sys/memory/mm_struct.h>
#include <sys/parser/parsetarfs.h>
#include <syscall.h>
#include <sys/memory/free_phys_pages.h>

/**
 * Kernel mm_struct
 */
static struct kernel_mm_struct kernel_mm = {
    {&(kernel_mm.mmap), &(kernel_mm.mmap)}, 0
};

/**
 * Head of list of mm_struct objects
 */
LIST_HEAD(head_mm);

/**
 * Initialize mm_struct object
 */
static void init_mm(struct mm_struct *mm) {
    (mm->mmap).next = (mm->mmap).prev = &(mm->mmap);
    mm->start_code = mm->end_code = 0x0ULL;
    mm->start_data = mm->end_data = 0x0ULL;
    mm->start_heap = mm->end_heap = 0x0ULL;
    mm->start_stack = mm->end_stack = 0x0ULL;
}

struct kernel_mm_struct* get_kernel_mm() {
    return &kernel_mm;
}

struct mm_struct* new_mm() {
    struct mm_struct *mm = (struct mm_struct*)
                                            kmalloc(sizeof(struct mm_struct));

    if(mm) init_mm(mm);
    return mm;
}

int init_data_vma(struct mm_struct *this, uint64_t start_data_a,
                        uint64_t end_data_a, uint64_t page_prot_a, int flags) {

    if(add_vma(&(this->mmap), start_data_a, end_data_a, page_prot_a, flags)
                                                                        == -1) {
        return -1;
    }

    this->start_data = start_data_a;
    this->end_data = end_data_a;

    return 0;
}

static int insert_vma_in_list(uint64_t start_addr_a, uint64_t end_addr_a,
            uint64_t page_prot_a, int flags, struct vm_area_struct *add_before,
                                            struct vm_area_struct *add_after) {

    if(!add_before && !add_after) {
        return -1;
    }

    // Check if existing vma's can be extended
    int extend_vma = 0;

    if(add_before && (add_before->vm_start == end_addr_a) &&
                                    (add_before->vm_page_prot == page_prot_a) &&
                                            (add_before->vm_flags == flags)) {
        add_before->vm_start = start_addr_a;
        extend_vma = 1;
    }

    if(add_after && (add_after->vm_end == start_addr_a) &&
                                     (add_after->vm_page_prot == page_prot_a) &&
                                             (add_after->vm_flags == flags)) {
        if(extend_vma == 1) {

            // Merge with add_before and remove add_after node
            add_before->vm_start = add_after->vm_start;

            ((add_after->vm_list).prev)->next = &(add_before->vm_list);
            (add_before->vm_list).prev = (add_after->vm_list).prev;

            kfree(add_after);

        } else {
            add_after->vm_end = end_addr_a;
            extend_vma = 1;
        }
    }

    if(extend_vma) {
        return 0;
    }

    // Existing vma not expanded. Create new
    struct vm_area_struct *p_vma = new_vma();

    if(!p_vma) {
        return -1;
    }

    p_vma->vm_start = start_addr_a;
    p_vma->vm_end = end_addr_a;
    p_vma->vm_page_prot = page_prot_a;
    p_vma->vm_flags = flags;

    if(add_before) {

        (p_vma->vm_list).prev = (add_before->vm_list).prev;
        (p_vma->vm_list).next = &(add_before->vm_list);
        ((add_before->vm_list).prev)->next = &(p_vma->vm_list);
        (add_before->vm_list).prev = &(p_vma->vm_list);

    } else if(add_after) {

        (p_vma->vm_list).next = (add_after->vm_list).next;
        (p_vma->vm_list).prev = &(add_after->vm_list);
        ((add_after->vm_list).next)->prev = &(p_vma->vm_list);
        (add_after->vm_list).next = &(p_vma->vm_list);

    }

    return 0;
}

int add_vma(struct list_head *mmap, uint64_t start_addr_a,
                        uint64_t end_addr_a, uint64_t page_prot_a, int flags) {

    // start addr < end addr
    if(start_addr_a >= end_addr_a) {
        return -1;
    }

#if 0
    // Page aligned addresses
    if(((uint64_t)start_addr_a & PG_ALN_CHK_MASK) ||
                                    ((uint64_t)end_addr_a & PG_ALN_CHK_MASK)) {
        return -1;
    }
#endif

    // List is empty
    if(list_empty(mmap)) {
        // Create new
        struct vm_area_struct *p_vma = new_vma();

        if(!p_vma) {
            return -1;
        }

        p_vma->vm_start = start_addr_a;
        p_vma->vm_end = end_addr_a;
        p_vma->vm_page_prot = page_prot_a;
        p_vma->vm_flags = flags;

        list_add_tail(&p_vma->vm_list, mmap);
        
        return 0;
    }

    int ret = -1;

    // Traverse list of vmas and
    // (1) Look for overlaps
    // (2) Maintain list sorted - ascending order

    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, mmap, vm_list) {

        // First node
        if((node->vm_list).prev == mmap) {
            if(end_addr_a <= node->vm_start) {
                ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                            flags, node, NULL);
                break;
            }
        }

        // Last node
        if((node->vm_list).next == mmap) {
            if(start_addr_a >= node->vm_end) {
                ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                            flags, NULL, node);
                break;
            }
        }

        struct vm_area_struct *prev_node = list_entry((node->vm_list).prev,
                                                struct vm_area_struct, vm_list);

        // In-between nodes
        if((start_addr_a >= prev_node->vm_end) &&
            (end_addr_a <= node->vm_start)) {

            ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                        flags, node, prev_node);
            break;
        }

    }

    return ret;
}

int init_code_vma(struct mm_struct *this, uint64_t start_code_a,
                        uint64_t end_code_a, uint64_t page_prot_a, int flags) {

    if(add_vma(&(this->mmap), start_code_a, end_code_a, page_prot_a, flags)
                                                                        == -1) {
        return -1;
    }

    this->start_code = start_code_a;
    this->end_code = end_code_a;

    return 0;
}

int init_heap_vma(struct mm_struct *this, uint64_t start_heap_a,
                        uint64_t end_heap_a, uint64_t page_prot_a, int flags) {

    if(add_vma(&(this->mmap), start_heap_a, end_heap_a, page_prot_a, flags)
                                                                        == -1) {
        return -1;
    }

    this->start_heap = start_heap_a;
    this->end_heap = end_heap_a;

    return 0;
}

int init_stack_vma(struct mm_struct *this, uint64_t start_stack_a,
                    uint64_t end_stack_a, uint64_t page_prot_a, int flags) {

    if(add_vma(&(this->mmap), start_stack_a, end_stack_a, page_prot_a, flags)
                                                                        == -1) {
        return -1;
    }

    this->start_stack = start_stack_a;
    this->end_stack = end_stack_a;

    return 0;
}

struct vm_area_struct* find_vma(struct list_head *mmap, uint64_t addr) {

    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, mmap, vm_list) {

        if(addr < node->vm_end) {
            break;
        }
    }

    return node;
}

struct vm_area_struct* find_vma_intersection(struct list_head *mmap,
                                    uint64_t start_addr, uint64_t end_addr) {

    struct vm_area_struct *node = find_vma(mmap, start_addr);
    if(node && end_addr <= node->vm_start) {
        node = NULL;
    }

    return node;
}

uint64_t get_unmapped_area(struct list_head *mmap, uint64_t start_addr,
                                                                uint64_t len) {

    // TODO: Limit start_addr+len based on start_addr in user/kernel space
    uint64_t addr = start_addr;

    struct vm_area_struct *node;

    for(node = find_vma(mmap, addr); (&node->vm_list) != mmap;
        node = list_entry((node->vm_list).next, struct vm_area_struct, vm_list))
    {
        if((addr + len) < node->vm_start) {
            break;
        } else {
            addr = node->vm_end;
        }
    }

    return addr;
}

void print_vmas(struct mm_struct *this) {

    struct vm_area_struct *node = NULL;

    printf("mm_struct: %p,", this);
    printf("&( ->mmap): %p,", &(this->mmap));
    printf("( ->mmap).next: %p,", (this->mmap).next);
    printf("( ->mmap).prev: %p,", (this->mmap).prev);
    printf("->start_code: %p,", this->start_code);
    printf("->end_code: %p,", this->end_code);
    printf("->start_data: %p,", this->start_data);
    printf("->end_data: %p,", this->end_data);
    printf("->start_heap: %p,", this->start_heap);
    printf("->end_heap: %p,", this->end_heap);
    printf("->start_stack: %p,", this->start_stack);
    printf("->end_stack: %p,", this->end_stack);
    printf("#");

    list_for_each_entry(node, &this->mmap, vm_list) {

        printf("vm_area_struct: %p,", node);
        printf("->vm_start: %p,", node->vm_start);
        printf("->vm_end: %p,", node->vm_end);
        printf("->vm_page_prot: %p,", node->vm_page_prot);
        printf("->vm_flags: %p,", node->vm_flags);
        printf("&( ->vm_list): %p,", &(node->vm_list));
        printf("( ->vm_list).next: %p,", (node->vm_list).next);
        printf("( ->vm_list).prev: %p,", (node->vm_list).prev);
        printf("#");

    }
}

extern struct phys_page_manager phys_page_mngr_obj;

int do_mmap(struct list_head *map, int file, uint64_t offset, uint64_t addr,
                                                uint64_t len, uint64_t prot) {

    // Number of physical pages
    int num_pages = len/PG_SZ + 1;

    // Virtual address
    uint64_t v_addr = mmap(map, addr, len, prot, 0, file, offset);

    if(v_addr != addr) {
        printf("Cannot load file at %p!\n", addr);
    }

    // Remaining bytes
    uint64_t bytes = len;

    for(int i = 0; i < num_pages; i++) {

        // Get free physical page
        uint64_t phys = alloc_phys_pages(1);

        // Add to page table
        update_curr_page_table(phys, v_addr, prot);

        // Load page with file contents
        sys_lseek(file, offset, SEEK_SET);
        uint64_t bytes_read = sys_read(file, (void*)v_addr,
                                    (bytes > SIZEOF_PAGE)? SIZEOF_PAGE : bytes);

        bytes -= bytes_read;
        v_addr += SIZEOF_PAGE;

    }

    return 0;

}

uint64_t mmap(struct list_head *mmap, uint64_t addr, uint64_t length,
                            uint64_t prot, int flags, int fd, uint64_t offset) {

    // TODO: Check return value
    uint64_t start_addr = get_unmapped_area(mmap, addr, length);

    if(-1 == add_vma(mmap, start_addr, start_addr + length, prot, flags)) {
        return -1;
    }

    // TODO: If not anonymous VMA, update vma with file backup information

    return start_addr;
}

static int copy_vmas(struct list_head *src, struct list_head *dest) {

    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, src, vm_list) {
        struct vm_area_struct *new_node = (struct vm_area_struct*)
                                        kmalloc(sizeof(struct vm_area_struct));

        if(!new_node) {
            // TODO: Free stuff!
            return -1;
        }

        *new_node = *node;
        list_add_tail(&new_node->vm_list, dest);
    }

    return 0;
}

struct kernel_mm_struct* cow_fork_kmm_struct() {

    struct kernel_mm_struct *src = get_kernel_mm();

    if(src == NULL) {
        return NULL;
    }

    struct kernel_mm_struct *dest = (struct kernel_mm_struct*)
                                    kmalloc(sizeof(struct kernel_mm_struct));

    if(!dest) {
        return NULL;
    }

    dest->start_kernel = src->start_kernel;
    dest->end_kernel = src->end_kernel;
    dest->start_vdo_buff = src->start_vdo_buff;
    dest->end_vdo_buff = src->end_vdo_buff;
    dest->start_phys_mem = src->start_phys_mem;
    dest->end_phys_mem = src->end_phys_mem;

    if(-1 == copy_vmas(&(src->mmap), &(dest->mmap))) {
        return NULL;
    }

    return dest;
}

struct mm_struct* cow_fork_mm_struct(struct mm_struct *src) {

    if(src == NULL) {
        return NULL;
    }

    struct mm_struct *dest = new_mm();

    if(!dest) {
        return NULL;
    }

    dest->start_code = src->start_code;
    dest->end_code = src->end_code;
    dest->start_data = src->start_data;
    dest->end_data = src->end_data;
    dest->start_heap = src->start_heap;
    dest->end_heap = src->end_heap;
    dest->start_stack = src->start_stack;
    dest->end_stack = src->end_stack;

    if(-1 == copy_vmas(&(src->mmap), &(dest->mmap))) {
        return NULL;
    }

    return dest;
}

