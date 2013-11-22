#ifndef _PAGE_CONSTANTS_H
#define _PAGE_CONSTANTS_H

// Number of bit shifts for multiplication/division by page size
#define LOG2_SIZEOF_PAGE 12

// Size of page, page frame
#define SIZEOF_PAGE (1UL << LOG2_SIZEOF_PAGE)

// Page alignment
#define PAGE_ALIGN (0xffffffffffffffffUL << LOG2_SIZEOF_PAGE)

#endif

