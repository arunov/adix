#include <sys/memory/vm_area_struct.h>

/**
 * Pool of vma objects for kernel
 */
static struct vm_area_struct kernel_pool_vma[KERNEL_VMA_COUNT] = {0};

/**
 * Number of vma objects allocated from pool
 */
static int count_kernel_pool_vma = 0;

struct vm_area_struct* new_kernel_vma() {
    if(count_kernel_pool_vma < KERNEL_VMA_COUNT) {
        count_kernel_pool_vma ++;

        return kernel_pool_vma + count_kernel_pool_vma - 1;
    } else {
        return new_vma();
    }
}

struct vm_area_struct* new_vma() {
    // TODO: kmalloc
    return NULL;
}

