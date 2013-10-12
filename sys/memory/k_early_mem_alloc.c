#include <sys/memory/k_early_mem_alloc.h>

void k_early_mem_alloc_init(struct k_early_mem_alloc *this,
                                    uint32_t *modulep,
                                    void *physbase, void *physfree) {
    this->modulep = modulep;
    this->physbase = physbase;
    this->physfree = physfree;
    this->addr = physfree;
}

void* get_next_mem_4Kb_align(struct k_early_mem_alloc *this,
                                uint32_t bytes) {
    void *freemem;

    if((uint64_t)(this->addr) & 0xfffUL) {
        freemem = (void*)(((uint64_t)(this->addr) & 0xfffffffffffff000UL) + 0x1000UL);
        this->addr = freemem + bytes;
    } else {
        freemem = this->addr;
        this->addr += bytes;
    }

    return freemem;
}

void* get_next_mem(struct k_early_mem_alloc *this, uint32_t bytes) {
    void *freemem = this->addr;
    this->addr += bytes;

    return freemem;
}

/*** Test ***/
int test_k_early_mem_alloc_init() {
    return 1;    
}

int test_get_next_mem_4Kb_align() {
    return 1;    
}

int test_get_next_mem() {
    return 1;    
}

/*** Test ***/

