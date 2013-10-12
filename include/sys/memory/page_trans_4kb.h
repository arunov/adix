#ifndef _PAGE_TRANS_4KB_H
#define _PAGE_TRANS_4KB_H

#include <defs.h>

struct page_trans_4Kb {
    uint64_t NX:1,
        available:11,
        p_PDPE_4Kb:40,
        AVL:3,
        bits6_8:3,
        A:1,
        PCD:1,
        PWT:1,
        US:1,
        RW:1,
        P:1;
};

#endif

