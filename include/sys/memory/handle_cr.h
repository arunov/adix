#ifndef _HANDLE_CR2_CR3_H
#define _HANDLE_CR2_CR3_H

#include <defs.h>

// CR0 bit 16: Protect read-only pages from supervisor-level writes
#define CR0_WP 0x10000UL

struct str_cr3 {
    uint64_t reserved_low:3,
        PWT:1,
        PCD:1,
        reserved_high:7,
        p_PML4E_4Kb:40,
        reserved_MBZ:12;
};

uint64_t get_cr0();
void* get_cr2();            // Returns virtual address
struct str_cr3 get_cr3();   // Returns cr3 containing physical address
uint64_t get_cr4();

void set_cr0(uint64_t val);
void set_cr3(struct str_cr3 page_trans);

struct str_cr3 get_default_cr3();

#endif

