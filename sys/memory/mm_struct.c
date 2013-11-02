#include <sys/memory/mm_struct.h>

/**
 * Pool of mm_struct objects for kernel
 */
static struct mm_struct kernel_pool_mm[KERNEL_MM_COUNT] = {0};

/**
 * Number of mm_struct objects allocated from pool
 */
static int count_kernel_pool_mm = 0;

/**
 * Pointer to mm_struct object for kernel
 */
static struct mm_struct *kernel_mm_struct;

/**
 * Pointer to head of mm_struct list
 */
static struct mm_struct *head_mm = NULL;

/**
 * Pointer to tail of mm_struct list
 */
static struct mm_struct *tail_mm = NULL;

struct mm_struct* new_kernel_mm() {
    if(count_kernel_pool_mm < KERNEL_MM_COUNT) {
        count_kernel_pool_mm ++;
        kernel_mm_struct = kernel_pool_mm + count_kernel_pool_mm - 1;

        // Add to list of mm_struct
        if(head_mm == NULL) {
            
        }
        return kernel_mm_struct;
    } else {
        return new_mm();
    }
}

struct mm_struct* get_kernel_mm() {
    if(count_kernel_pool_mm) {
        return kernel_mm_struct;
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
    this->start_data = start_data_a;
    this->end_data = end_data_a;


}

