#ifndef _K_INITIAL_MEMORY_HELPER_H
#define _K_INITIAL_MEMORY_HELPER_H

struct k_initial_memory_helper {
    uint32_t *modulep;
    void *physbase, *physfree;

    // Private
    void *addr;
}

void* get_next_4Kb_aligned(uint32_t bytes);
void* get_next_memory(uint32_t bytes);

#endif

