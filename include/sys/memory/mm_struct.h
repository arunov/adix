#ifndef _MM_STRUCT_H
#define _MM_STRUCT_H

#include <defs.h>
#include <sys/memory/vm_area_struct.h>

#define KERNEL_VMA_COUNT 4
#define KERNEL_MM_COUNT 1

/* Process memory descriptor */
struct mm_struct {
    /* Pointer to virtual memory areas */
    struct vm_area_struct *mmap;

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

    /* List of mm_struct addresses */
    struct mm_struct *next;

};

/**
 * Return new mm_struct possibly for a new process
 */
struct mm_struct* new_mm();

/**
 * Return mm_struct for kernel
 */
struct mm_struct* get_kernel_mm();

/**
 * Return new mm_struct for a kernel process
 */
struct mm_struct* new_kernel_mm();

/**
 * Initialize data vma
 * @param this         pointer to mm_struct
 * @param start_data_a starting virtual address of data section
 * @param end_data_a   ending virtual address of data section
 * @param page_prot    page protection
 * @return OK or ERROR
 */
int init_data_vma(struct mm_struct *this, uint64_t start_data_a,
                                    uint64_t end_data_a, uint64_t page_prot_a);



#endif

