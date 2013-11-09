#include <sys/memory/vm_area_struct.h>

/**
 * Pool of vma objects
 */
static struct vm_area_struct vma_pool[VMA_POOL_COUNT] = {{0}};

/**
 * Number of vma objects allocated from pool
 */
static int vma_pool_use_count = 0;

struct vm_area_struct* new_vma() {

    if(vma_pool_use_count < VMA_POOL_COUNT) {
        vma_pool_use_count ++;

        return &vma_pool[vma_pool_use_count - 1];
    } else {
        // TODO: kmalloc
        return NULL;
    }
}

