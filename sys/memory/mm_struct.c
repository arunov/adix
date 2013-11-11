#include <sys/memory/mm_struct.h>

/**
 * Pool of mm_struct objects for kernel
 */
static struct mm_struct kernel_pool_mm[KERNEL_MM_COUNT] = {{{0}}};

/**
 * Number of mm_struct objects allocated from pool
 */
static int count_kernel_pool_mm = 0;

/**
 * Pointer to mm_struct object for kernel
 */
static struct mm_struct *kernel_mm[KERNEL_MM_COUNT] = {0};

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
    mm->start_stack = 0x0ULL;
}

struct mm_struct* new_kernel_mm() {

    struct mm_struct *p_mm = NULL;

    if(count_kernel_pool_mm < KERNEL_MM_COUNT) {

        // Using up one more mm object from kernel pool
        count_kernel_pool_mm ++;

        // Object from kernel pool;
        p_mm = kernel_mm[count_kernel_pool_mm - 1]
                                    = &kernel_pool_mm[count_kernel_pool_mm - 1];

    } else {
        p_mm = new_mm();
    }

    // Add to list of mm_struct
    if(p_mm) {
        init_mm(p_mm);
        list_add_tail(&p_mm->mm_list, &head_mm);
    }

    return p_mm;
}

struct mm_struct* get_kernel_mm(int id) {

    if((id < KERNEL_MM_COUNT) && kernel_mm[id]) {
        return kernel_mm[id];
    } else {
        return NULL;
    }
}

struct mm_struct* new_mm() {
    // TODO: kmalloc
    return NULL;
}

int init_data_vma(struct mm_struct *this, uint64_t start_data_a,
                                    uint64_t end_data_a, uint64_t page_prot_a) {

    if(add_vma(this, start_data_a, end_data_a, page_prot_a) == -1) {
        return -1;
    }

    this->start_data = start_data_a;
    this->end_data = end_data_a;

    return 0;
}

static int insert_vma_in_list(uint64_t start_addr_a, uint64_t end_addr_a,
                        uint64_t page_prot_a, struct vm_area_struct *add_before,
                        struct vm_area_struct *add_after) {

    if(!add_before && !add_after) {
        return -1;
    }

    // Check if existing vma's can be extended
    int extend_vma = 0;

    if(add_before && (add_before->vm_start == end_addr_a) &&
                                    (add_before->vm_page_prot == page_prot_a)) {
        add_before->vm_start = start_addr_a;
        extend_vma = 1;
    }

    if(add_after && (add_after->vm_end == start_addr_a) &&
                                    (add_after->vm_page_prot == page_prot_a)) {
        if(extend_vma == 1) {

            // Merge with add_before and remove add_after node
            add_before->vm_start = add_after->vm_start;

            struct vm_area_struct *node_before = list_entry(
                    (add_after->vm_list).prev, struct vm_area_struct, vm_list);

            (node_before->vm_list).next = &(add_before->vm_list);
            (add_before->vm_list).prev = &(node_before->vm_list);

            // TODO: Free add_after

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

int add_vma(struct mm_struct *this, uint64_t start_addr_a,
                                    uint64_t end_addr_a, uint64_t page_prot_a) {

    // start addr < end addr
    if(start_addr_a >= end_addr_a) {
        return -1;
    }

    // Page aligned addresses
    if(((uint64_t)start_addr_a & PG_ALN_CHK_MASK) ||
                                    ((uint64_t)end_addr_a & PG_ALN_CHK_MASK)) {
        return -1;
    }

    // List is empty
    if(list_empty(&this->mmap)) {
        // Create new
        struct vm_area_struct *p_vma = new_vma();

        if(!p_vma) {
            return -1;
        }

        p_vma->vm_start = start_addr_a;
        p_vma->vm_end = end_addr_a;
        p_vma->vm_page_prot = page_prot_a;

        list_add_tail(&p_vma->vm_list, &this->mmap);
        
        return 0;
    }

    int ret = -1;

    // Traverse list of vmas and
    // (1) Look for overlaps
    // (2) Maintain list sorted - ascending order

    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, &this->mmap, vm_list) {

        // First node
        if((node->vm_list).prev == &this->mmap) {
            if(end_addr_a <= node->vm_start) {
                ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                                    node, NULL);
                break;
            }
        }

        // Last node
        if((node->vm_list).next == &this->mmap) {
            if(start_addr_a >= node->vm_end) {
                ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                                    NULL, node);
                break;
            }
        }

        struct vm_area_struct *prev_node = list_entry((node->vm_list).prev,
                                                struct vm_area_struct, vm_list);

        // In-between nodes
        if((start_addr_a >= prev_node->vm_end) &&
            (end_addr_a <= node->vm_start)) {

            ret = insert_vma_in_list(start_addr_a, end_addr_a, page_prot_a,
                                                            node, prev_node);
            break;
        }

    }

    return ret;
}

int init_code_vma(struct mm_struct *this, uint64_t start_code_a,
                                    uint64_t end_code_a, uint64_t page_prot_a) {

    if(add_vma(this, start_code_a, end_code_a, page_prot_a) == -1) {
        return -1;
    }

    this->start_code = start_code_a;
    this->end_code = end_code_a;

    return 0;
}

int init_heap_vma(struct mm_struct *this, uint64_t start_heap_a,
                                    uint64_t end_heap_a, uint64_t page_prot_a) {

    if(add_vma(this, start_heap_a, end_heap_a, page_prot_a) == -1) {
        return -1;
    }

    this->start_heap = start_heap_a;
    this->end_heap = end_heap_a;

    return 0;
}

int init_stack_vma(struct mm_struct *this, uint64_t start_stack_a,
                                uint64_t end_stack_a, uint64_t page_prot_a) {

    if(add_vma(this, start_stack_a, end_stack_a, page_prot_a) == -1) {
        return -1;
    }

    this->start_stack = start_stack_a;
    this->end_stack = end_stack_a;

    return 0;
}

struct vm_area_struct* find_vma(struct mm_struct *this, uint64_t addr) {

    struct vm_area_struct *node = NULL;

    list_for_each_entry(node, &this->mmap, vm_list) {

        if(addr < node->vm_end) {
            break;
        }
    }

    return node;
}

struct vm_area_struct* find_vma_intersection(struct mm_struct *this,
                                    uint64_t start_addr, uint64_t end_addr) {

    struct vm_area_struct *node = find_vma(this, start_addr);
    if(node && end_addr <= node->vm_start) {
        node = NULL;
    }

    return node;
}

uint64_t get_unmapped_area(struct mm_struct *this, uint64_t start_addr,
                                                                uint64_t len) {

    // TODO: Limit start_addr+len based on start_addr in user/kernel space
    uint64_t addr = start_addr;

    struct vm_area_struct *node;

    for(node = find_vma(this, addr) ; ; ) {

        if(!node || ((addr + len) < node->vm_start)) {
            break;
        } else {
            addr = node->vm_end;
        }

        node = list_entry((node->vm_list).next, struct vm_area_struct, vm_list);
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
    printf("( ->mm_list).next: %p,", (this->mm_list).next);
    printf("( ->mm_list).prev: %p,", (this->mm_list).prev);
    printf("#");

    list_for_each_entry(node, &this->mmap, vm_list) {

        printf("vm_area_struct: %p,", node);
        printf("->vm_start: %p,", node->vm_start);
        printf("->vm_end: %p,", node->vm_end);
        printf("->vm_page_prot: %p,", node->vm_page_prot);
        printf("&( ->vm_list): %p,", &(node->vm_list));
        printf("( ->vm_list).next: %p,", (node->vm_list).next);
        printf("( ->vm_list).prev: %p,", (node->vm_list).prev);
        printf("#");

    }
}

