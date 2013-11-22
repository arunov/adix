#ifndef _MM_STRUCT_H
#define _MM_STRUCT_H

#include <defs.h>
#include <sys/list.h>
#include <sys/kstdio.h>
#include <sys/memory/vm_area_struct.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/page_table_helper.h>
#include <sys/memory/kmalloc.h>

/* Process memory descriptor */
struct mm_struct {

    /* Pointer to virtual memory areas */
	struct list_head mmap;

    /* Starting virtual address of code section */
    uint64_t start_code;

    /* Ending virtual address of code section */
    uint64_t end_code;

    /* Starting virtual address of data section */
    uint64_t start_data;

    /* Ending virtual address of data section */
    uint64_t end_data;

    /* Starting virtual address of heap section */
    uint64_t start_heap;

    /* Ending virtual address of heap section */
    uint64_t end_heap;

    /* Starting virtual address of user stack section */
    uint64_t start_stack;

    /* Ending virtual address of user stack section */
    uint64_t end_stack;

    /* List of mm_struct */
    struct list_head mm_list;

};

/* Virtual memory map for kernel */
struct kernel_mm_struct {

    /* Pointer to virtual memory areas */
	struct list_head mmap;

    /***************************************************************************
     * vma can be added using add_kernel_vma().
     * Other members - start and end addresses of kernel, video buffer, and map
     * of physical memory - are to be updated directly.
     **************************************************************************/

    /* Starting virtual address of kernel */
    uint64_t start_kernel;

    /* Ending virtual address of kernel */
    uint64_t end_kernel;

    /* Starting virtual address of video buffer */
    uint64_t start_vdo_buff;

    /* Ending virtual address of video buffer */
    uint64_t end_vdo_buff;

    /* Starting virtual address of physical memory mapped in kernel space */
    //uint64_t start_phys_mem;

    /* Ending virtual address of physical memory mapped in kernel space */
    //uint64_t end_phys_mem;

};

/**
 * Return new mm_struct possibly for a new process
 */
struct mm_struct* new_mm();

/**
 * Return mm_struct for kernel
 */
struct kernel_mm_struct* get_kernel_mm();

/**
 * Initialize vma
 * @param mmap         pointer to vma list
 * @param start_addr_a starting virtual address of section
 * @param end_addr_a   ending virtual address of section
 * @param page_prot    page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                     PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                     (@see include/sys/memory/page_table_helper.h).
 * @param flags        MAP_ANONYMOUS - no file backup
 *                     MAP_GROWSDOWN - region can expand towards lower addresses
 *                     (@see include/sys/memory/vm_area_struct.h)
 * @return             OK or ERROR
 */
int add_vma(struct list_head *mmap, uint64_t start_addr_a,
                        uint64_t end_addr_a, uint64_t page_prot_a, int flags);

/**
 * Initialize data vma
 * @param this         pointer to mm_struct
 * @param start_data_a starting virtual address of data section
 * @param end_data_a   ending virtual address of data section
 * @param page_prot    page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                     PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                     (@see include/sys/memory/page_table_helper.h).
 * @param flags        MAP_ANONYMOUS - no file backup
 *                     MAP_GROWSDOWN - region can expand towards lower addresses
 *                     (@see include/sys/memory/vm_area_struct.h)
 * @return             OK or ERROR
 */
int init_data_vma(struct mm_struct *this, uint64_t start_data_a,
                        uint64_t end_data_a, uint64_t page_prot_a, int flags);

/**
 * Initialize code vma
 * @param this         pointer to mm_struct
 * @param start_code_a starting virtual address of code section
 * @param end_code_a   ending virtual address of code section
 * @param page_prot    page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                     PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                     (@see include/sys/memory/page_table_helper.h).
 * @param flags        MAP_ANONYMOUS - no file backup
 *                     MAP_GROWSDOWN - region can expand towards lower addresses
 *                     (@see include/sys/memory/vm_area_struct.h)
 * @return             OK or ERROR
 */
int init_code_vma(struct mm_struct *this, uint64_t start_code_a,
                        uint64_t end_code_a, uint64_t page_prot_a, int flags);

/**
 * Initialize heap vma
 * @param this         pointer to mm_struct
 * @param start_heap_a starting virtual address of heap section
 * @param end_heap_a   ending virtual address of heap section
 * @param page_prot    page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                     PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                     (@see include/sys/memory/page_table_helper.h).
 * @param flags        MAP_ANONYMOUS - no file backup
 *                     MAP_GROWSDOWN - region can expand towards lower addresses
 *                     (@see include/sys/memory/vm_area_struct.h)
 * @return             OK or ERROR
 */
int init_heap_vma(struct mm_struct *this, uint64_t start_heap_a,
                        uint64_t end_heap_a, uint64_t page_prot_a, int flags);

/**
 * Initialize stack vma
 * @param this          pointer to mm_struct
 * @param start_stack_a starting virtual address of stack section
 * @param end_stack_a   ending virtual address of stack section
 * @param page_prot     page protection. E.g. PAGE_TRANS_READ_WRITE |
 *                      PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *                      (@see include/sys/memory/page_table_helper.h).
 * @param flags         MAP_ANONYMOUS - no file backup
 *                      MAP_GROWSDOWN - region can expand towards lower
 *                                      addresses
 *                      (@see include/sys/memory/vm_area_struct.h)
 * @return              OK or ERROR
 */
int init_stack_vma(struct mm_struct *this, uint64_t start_stack_a,
                        uint64_t end_stack_a, uint64_t page_prot_a, int flags);

/**
 * Find first vma whose vm_end is greater than the address 'addr'
 * @param mmap pointer to vma list
 * @param addr virtual address to find vma
 * @return     first vma whose vm_end > addr or NULL
 */
struct vm_area_struct* find_vma(struct list_head *mmap, uint64_t addr);

/**
 * Find a region that overlaps a given interval
 * @param mmap       pointer to vma list
 * @param start_addr start address of interval
 * @param end_addr   end address of interval
 * @return           first vma that overlaps given interval or NULL
 */
struct vm_area_struct* find_vma_intersection(struct list_head *mmap,
                                        uint64_t start_addr, uint64_t end_addr);

/**
 * Get unmapped area
 * @param mmap       pointer to vma list
 * @param start_addr start address to start search
 * @param len        length of unmapped region
 * @return           start address of unmapped region or NULL
 */
uint64_t get_unmapped_area(struct list_head *mmap, uint64_t start_addr,
                                                                uint64_t len);

/**
 * Print virtual memory areas
 * @param this pointer to mm_struct
 */
void print_vmas(struct mm_struct *this);

/**
 * Create memory region
 * @param mmap   pointer to vma list
 * @param addr   virtual start address of memory region
 * @param length length of memory region
 * @param prot   page protection. E.g. PAGE_TRANS_READ_WRITE |
 *               PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *               (@see include/sys/memory/page_table_helper.h).
 * @param flags  MAP_ANONYMOUS - no file backup
 *               MAP_GROWSDOWN - region can expand towards lower addresses
 *               (@see include/sys/memory/vm_area_struct.h)
 * @param fd     file descriptor of file
 * @param offset offset in file
 * @return       address that is allocated for memory region
 */
uint64_t mmap(struct list_head *mmap, uint64_t addr, uint64_t length,
                            uint64_t prot, int flags, int fd, uint64_t offset);

/**
 * Create memory region
 * @param mmap   pointer to vma list
 * @param file   file descriptor of file
 * @param offset offset in file for start of memory region
 * @param addr   virtual address of memory region
 * @param len    length of memory
 * @param prot   page protection. E.g. PAGE_TRANS_READ_WRITE |
 *               PAGE_TRANS_USER_SUPERVISOR | PAGE_TRANS_NX
 *               (@see include/sys/memory/page_table_helper.h).
 * @retrun       OK or ERROR
 */
int do_mmap(struct list_head *mmap, int file, uint64_t offset, uint64_t addr,
                                                uint64_t len, uint64_t prot);

#endif

