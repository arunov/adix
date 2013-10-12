#ifndef _K_EARLY_MEM_ALLOC_H
#define _K_EARLY_MEM_ALLOC_H

#include <defs.h>

struct k_early_mem_alloc {
    // Private
    uint32_t *modulep;
    void *physbase, *physfree;
    void *addr;
};

void k_early_mem_alloc_init(struct k_early_mem_alloc *this,
                                    uint32_t *modulep,
                                    void *physbase, void *physfree);

void* get_next_mem_4Kb_align(struct k_early_mem_alloc *this,
                                uint32_t bytes);

void* get_next_mem(struct k_early_mem_alloc *this, uint32_t bytes);

/*** Test ***/
int test_k_early_mem_alloc_init();
int test_get_next_mem_4Kb_align();
int test_get_next_mem();
/*** Test ***/

#endif

