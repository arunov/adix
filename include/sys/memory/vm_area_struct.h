#ifndef _VM_AREA_STRUCT_H
#define _VM_AREA_STRUCT_H

#include <sys/list.h>

#define VMA_POOL_COUNT 4

/* Virtual memory area structure */
struct vm_area_struct {
    /* Pointer to mm_struct associated with vma */
    //struct mm_struct *vm_mm;

    /* Starting virtual address of virtual memory area */
    uint64_t vm_start;

    /* End virtual address of virtual memory area */
    uint64_t vm_end;

    /* Next vma object */
    //struct vm_area_struct *next;

    /* Protection bits for  */
    char vm_page_prot;

    /* List of vmas */
    struct list_head vm_list;
};

/**
 * Return new vm_area_struct
 */
struct vm_area_struct* new_vma();

#endif

