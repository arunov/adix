#ifndef _MM_STRUCT_H
#define _MM_STRUCT_H

#include <defs.h>
#include <sys/list.h>
#include <sys/kstdio.h>
#include <sys/memory/vm_area_struct.h>
#include <sys/memory/phys_page_manager.h>

#define KERNEL_MM_COUNT 1

#define EXPAND_VMA_NORMAL 1
#define EXPAND_VMA_REVERSE 2

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

    /* List of mm_struct */
    struct list_head mm_list;

};

/**
 * Return new mm_struct possibly for a new process
 */
struct mm_struct* new_mm();

/**
 * Return mm_struct for kernel
 */
struct mm_struct* get_kernel_mm(int id);

/**
 * Return new mm_struct for a kernel process
 */
struct mm_struct* new_kernel_mm();

/**
 * Initialize vma
 * @param this         pointer to mm_struct
 * @param start_addr_a starting virtual address of section
 * @param end_addr_a   ending virtual address of section
 * @param page_prot    page protection
 * @return             OK or ERROR
 */
int add_vma(struct mm_struct *this, uint64_t start_addr_a,
                                    uint64_t end_addr_a, uint64_t page_prot_a);

/**
 * Initialize data vma
 * @param this         pointer to mm_struct
 * @param start_data_a starting virtual address of data section
 * @param end_data_a   ending virtual address of data section
 * @param page_prot    page protection
 * @return             OK or ERROR
 */
int init_data_vma(struct mm_struct *this, uint64_t start_data_a,
                                    uint64_t end_data_a, uint64_t page_prot_a);

/**
 * Initialize code vma
 * @param this         pointer to mm_struct
 * @param start_code_a starting virtual address of code section
 * @param end_code_a   ending virtual address of code section
 * @param page_prot    page protection
 * @return             OK or ERROR
 */
int init_code_vma(struct mm_struct *this, uint64_t start_code_a,
                                    uint64_t end_code_a, uint64_t page_prot_a);

/**
 * Initialize heap vma
 * @param this         pointer to mm_struct
 * @param start_heap_a starting virtual address of heap section
 * @param end_heap_a   ending virtual address of heap section
 * @param page_prot    page protection
 * @return             OK or ERROR
 */
int init_heap_vma(struct mm_struct *this, uint64_t start_heap_a,
                                    uint64_t end_heap_a, uint64_t page_prot_a);

/**
 * Initialize stack vma
 * @param this          pointer to mm_struct
 * @param start_stack_a starting virtual address of stack section
 * @param end_stack_a   ending virtual address of stack section
 * @param page_prot     page protection
 * @return              OK or ERROR
 */
int init_stack_vma(struct mm_struct *this, uint64_t start_stack_a,
                                    uint64_t end_stack_a, uint64_t page_prot_a);

/**
 * Find first vma whose vm_end is greater than the address 'addr'
 * @param this pointer to mm_struct
 * @param addr virtual address to find vma
 * @return     first vma whose vm_end > addr or NULL
 */
struct vm_area_struct* find_vma(struct mm_struct *this, uint64_t addr);

/**
 * Find a region that overlaps a given interval
 * @param this       pointer to mm_struct
 * @param start_addr start address of interval
 * @param end_addr   end address of interval
 * @return           first vma that overlaps given interval or NULL
 */
struct vm_area_struct* find_vma_intersection(struct mm_struct *this,
                                        uint64_t start_addr, uint64_t end_addr);

/**
 * Get unmapped area
 * @param this       pointer to mm_struct
 * @param start_addr start address to start search
 * @param len        length of unmapped region
 * @return           start address of unmapped region or NULL
 */
uint64_t get_unmapped_area(struct mm_struct *this, uint64_t start_addr,
                                                                uint64_t len);

/**
 * Print virtual memory areas
 * @param this pointer to mm_struct
 */
void print_vmas(struct mm_struct *this);

#endif

