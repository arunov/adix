#ifndef _PAGE_TRANS_4KB_H
#define _PAGE_TRANS_4KB_H

#include <defs.h>

struct page_trans_4Kb {
    uint64_t P:1,
	RW:1,
	US:1,
	PWT:1,
	PCD:1,
	A:1,
	bits6_8:3,
	AVL:3,
	p_PDPE_4Kb:40,
	available:11, 
	NX:1;
};

#endif

