#ifndef _PAGE_TABLE_HELPER_H
#define _PAGE_TABLE_HELPER_H

/*
 * NOTE: Only 4KB page size is supported by this operating system
 */

#include <defs.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/kstdio.h> // TODO: Replace with logger
#include <sys/kstring.h>

// Number of entries in page translation objects
#define NUM_PAGE_TRANS_ENTRIES 512

// Size of page translation object in bytes
#define SIZEOF_PAGE_TRANS 4096


/*********************** Page translation object fields ***********************/
// Physical address of next level object or physical page Bits - 12 to 51
#define MASK_PAGE_TRANS_NEXT_LEVEL_ADDR 0x000FFFFFFFFFF000ULL
#define MASK_PAGE_TRANS_ADDR_SIGN_BIT 0x0008000000000000ULL
#define PAGE_TRANS_NEXT_LEVEL_ADDR(a) ((a) & MASK_PAGE_TRANS_NEXT_LEVEL_ADDR)
#define PAGE_TRANS_ADDR_SIGN_EXT(a) (((a) & MASK_PAGE_TRANS_ADDR_SIGN_BIT)? \
                                                0xFFF0000000000000ULL : 0ULL)

// Non address fields
#define PAGE_TRANS_NON_ADDR_FIELDS(a) ((a) & (~MASK_PAGE_TRANS_NEXT_LEVEL_ADDR))

// Present Bit - 0
#define PAGE_TRANS_PRESENT 0x1ULL

// Read/Write (R/W) Bit - 1
#define PAGE_TRANS_READ_WRITE 0x2ULL

// User/Supervisor (U/S) Bit - 2
#define PAGE_TRANS_USER_SUPERVISOR 0x4ULL

// Accessed (A) Bit - 5
#define PAGE_TRANS_ACCESSED 0x20ULL

// Dirty (D) Bit - 6
#define PAGE_TRANS_DIRTY 0x40ULL

// Page Size (PS) Bit - 7: Only 4KB supported
// CAUTION: Do not bitwise or this to Page Translation entries
#define MASK_PAGE_TRANS_PAGE_SIZE 0x80ULL

// No Execute (NX) Bit - 63
#define PAGE_TRANS_NX 0x8000000000000000ULL

/*************** Kinda not used ***************/
// Page-Level Writethrough (PWT) Bit - 3
#define PAGE_TRANS_PWT 0x8ULL

// Page-Level Cache Disable (PCD) Bit - 4
#define PAGE_TRANS_PCD 0x10ULL

// Global Page (G) Bit - 8
#define PAGE_TRANS_GLOBAL_PAGE 0x100ULL

// Page-Attribute Table (PAT) Bit - 7: For PTs, 4KB pages
#define PAGE_TRANS_PT_PAT 0x80ULL
/**********************************************/
/******************************************************************************/


/************************** Page translation indices **************************/
// Mask bits 47 to 39 of an 8 byte value 'a'
#define MASK_BITS_47_39(a) ((a) & 0x0000ff8000000000ULL)

// PML4E index of address 'a'
#define INDEX_PML4E(a) (MASK_BITS_47_39(a) >> 39)

// Mask bits 38 to 30 of an 8 byte value 'a'
#define MASK_BITS_38_30(a) ((a) & 0x0000007fc0000000ULL)

// PDPE index of address 'a'
#define INDEX_PDPE(a) (MASK_BITS_38_30(a) >> 30)

// Mask bits 29 to 21 of an 8 byte value 'a'
#define MASK_BITS_29_21(a) ((a) & 0x000000003fe00000ULL)

// PDE index of address 'a'
#define INDEX_PDE(a) (MASK_BITS_29_21(a) >> 21)

// Mask bits 20 to 12 of an 8 byte value 'a'
#define MASK_BITS_20_12(a) ((a) & 0x00000000001ff000ULL)

// PTE index of address 'a'
#define INDEX_PTE(a) (MASK_BITS_20_12(a) >> 12)
/******************************************************************************/


/****************************** Self referencing ******************************/
// Self reference entry in PML4 (9 bits)
#define SELF_REF_ENTRY 256ULL

// First 16 bits of virtual address of self referenced page table structures
#define SELF_REF_ADDR_BIT_63_48 (((SELF_REF_ENTRY & 0x100ULL)? \
                                    0xffffULL : 0x0000ULL) << 48)

// Self referenced virtual address for PML4 structure object corresponding to
// address 'a'
#define SELF_REF_PML4(a) ( (SELF_REF_ADDR_BIT_63_48) | \
                            (SELF_REF_ENTRY << 39) | \
                            (SELF_REF_ENTRY << 30) | \
                            (SELF_REF_ENTRY << 21) | \
                            (SELF_REF_ENTRY << 12) )

// Self referenced virtual address for PML4 entry corresponding to address 'a'
#define SELF_REF_PML4E(a) ( SELF_REF_PML4(a) | \
                            (MASK_BITS_47_39(a) >> 36) )

// Self referenced virtual address for PDP structure object corresponding to
// address 'a'
#define SELF_REF_PDP(a) ( (SELF_REF_ADDR_BIT_63_48) | \
                            (SELF_REF_ENTRY << 39) | \
                            (SELF_REF_ENTRY << 30) | \
                            (SELF_REF_ENTRY << 21) | \
                            (MASK_BITS_47_39(a) >> 27) )

// Self referenced virtual address for PDP entry corresponding to address 'a'
#define SELF_REF_PDPE(a) ( SELF_REF_PDP(a) | \
                            (MASK_BITS_38_30(a) >> 27) )

// Self referenced virtual address for PD structure object corresponding to
// address 'a'
#define SELF_REF_PD(a) ( (SELF_REF_ADDR_BIT_63_48) | \
                            (SELF_REF_ENTRY << 39) | \
                            (SELF_REF_ENTRY << 30) | \
                            (MASK_BITS_47_39(a) >> 18) | \
                            (MASK_BITS_38_30(a) >> 18) )

// Self referenced virtual address for PDP entry corresponding to address 'a'
#define SELF_REF_PDE(a) ( SELF_REF_PD(a) | \
                            (MASK_BITS_29_21(a) >> 18) )

// Self referenced virtual address for PT structure object corresponding to
// address 'a'
#define SELF_REF_PT(a) ( (SELF_REF_ADDR_BIT_63_48) | \
                            (SELF_REF_ENTRY << 39) | \
                            (MASK_BITS_47_39(a) >> 9) | \
                            (MASK_BITS_38_30(a) >> 9) | \
                            (MASK_BITS_29_21(a) >> 9) )

// Self referenced virtual address for PDP entry corresponding to address 'a'
#define SELF_REF_PTE(a) ( SELF_REF_PT(a) | \
                            (MASK_BITS_20_12(a) >> 9) )
/******************************************************************************/


/**
 * Helper for page translation objects
 */
struct page_table_helper{
    struct phys_page_manager *phys_mgr;

    /**
     * Base address of virtual memory where all of physical memory is mapped.
     * Initialized with 0 for identity mapping.
     */
    uint64_t phys_mem_virt_map_base;
};

// Virtual address corresponding to physical address 'a'
#define VIRTUAL_ADDR(a) ((uint64_t)((char*)(a) + \
                                                (this->phys_mem_virt_map_base)))

/**
 * Initialize page table helper with free page manager and identity mapping.
 * @param this              pointer to 'page_table_helper' object
 * @param a_phys_page_mgr   physical page manager used to get free pages
 */
void init_page_table_helper(struct page_table_helper *this,
                                    struct phys_page_manager *a_phys_page_mgr);

/**
 * Set base address of virtual area where all of physical memory is mapped.
 * @param this                      pointer to 'page_table_helper' object
 * @param a_phys_mem_virt_map_base  base address of virtual area
 */
void set_phys_mem_virt_map_base(struct page_table_helper *this,
                                            uint64_t a_phys_mem_virt_map_base);

/**
 * Get zeroed page translation object.
 * @param this  pointer to 'page_table_helper' object
 * @return  address of newly created page translation object
 */
uint64_t get_zeroed_page_trans_obj(struct page_table_helper *this);

/**
 * Get PML4 object that has a self reference entry.
 * @param this  pointer to 'page_table_helper' object
 * @return  address of newly created PML4 object
 */
uint64_t get_selfref_PML4(struct page_table_helper *this);

/**
 * Get duplicate PML4 object.
 * @param this  pointer to 'page_table_helper' object
 * @param src   PML4 object that needs to be duplicated
 */
uint64_t get_duplicate_PML4(struct page_table_helper *this, uint64_t src);

/**
 * Map physical address to virtual address with required permissions in PML4
 * @param this          pointer to 'page_table_helper' object
 * @param pml4          address of PML4 in which mapping is to be done
 * @param phys          physical address to map virtual address 'virt' with
 * @param virt          virtual address that should be mapped to 'phys'
 * @param access_perm   access permissions ORed. For e.g.: PAGE_TRANS_READ_WRITE
 *                      | PAGE_TRANS_USER_SUPERVISOR
 */
void update_page_table(struct page_table_helper *this, uint64_t pml4,
                            uint64_t phys, uint64_t virt, uint64_t access_perm);

/**
 * Map physical address to virtual address with required permissions in PML4
 * loaded in CR3. This function does NOT assume identity mapping or virtual
 * address mapped with all physical addresses
 * @param this          pointer to 'page_table_helper' object
 * @param phys          physical address to map virtual address 'virt' with
 * @param virt          virtual address that should be mapped to 'phys'
 * @param access_perm   access permissions ORed. For e.g.: PAGE_TRANS_READ_WRITE
 *                      | PAGE_TRANS_USER_SUPERVISOR
 */
void update_curr_page_table(struct page_table_helper *this, uint64_t phys,
                                        uint64_t virt, uint64_t access_perm);

#endif

