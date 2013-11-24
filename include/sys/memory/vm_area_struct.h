#ifndef _VM_AREA_STRUCT_H
#define _VM_AREA_STRUCT_H

#include <sys/list.h>

#define MAP_ANONYMOUS 0x00000001U
#define MAP_GROWSDOWN 0x00000002U

/* Virtual memory area structure */
struct vm_area_struct {

    /* Starting virtual address of virtual memory area */
    uint64_t vm_start;

    /* End virtual address of virtual memory area */
    uint64_t vm_end;

    /* Protection bits for  */
    uint64_t vm_page_prot;

    /**
     * Flags for memory area
     * MAP_ANONYMOUS - no file backup
     * MAP_GROWSDOWN - region can expand towards lower addresses
     */
    int vm_flags;

    /* List of vmas */
    struct list_head vm_list;
};

/**
 * Return new vm_area_struct
 */
struct vm_area_struct* new_vma();

#endif

