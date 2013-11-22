#ifndef _HANDLE_CR2_CR3_H
#define _HANDLE_CR2_CR3_H

#include <defs.h>

struct str_cr3 {
    uint64_t reserved_low:3,
        PWT:1,
        PCD:1,
        reserved_high:7,
        p_PML4E_4Kb:40,
        reserved_MBZ:12;
};

void* get_cr2();            // Returns virtual address
struct str_cr3 get_cr3();   // Returns cr3 containing physical address
void set_cr3(struct str_cr3 page_trans);

struct str_cr3 get_default_cr3();

#endif

