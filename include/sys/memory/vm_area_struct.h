#ifndef _VM_AREA_STRUCT_H
#define _VM_AREA_STRUCT_H

/* Virtual memory area structure */
struct vm_area_struct {
    /* Pointer to mm_struct associated with vma */
    //struct mm_struct *vm_mm;

    /* Starting virtual address of virtual memory area */
    uint64_t vm_start;

    /* End virtual address of virtual memory area */
    uint64_t vm_end;

    /* Next vma object */
    struct vm_area_struct *next;

    /* Protection bits for  */
    char vm_page_prot;
};

/**
 * Return new vm_area_struct
 */
struct vm_area_struct* new_vma();

/**
 * Return new vm_area_struct for a kernel process
 */
struct mm_struct* new_kernel_vma();

#endif

