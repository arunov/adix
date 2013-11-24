#ifndef _SETUP_KERNEL_MEMORY_H
#define _SETUP_KERNEL_MEMORY_H

#include <sys/memory/mm_struct.h>
#include <defs.h>
#include <sys/memory/page_constants.h>

#define SIZEOF_VDO_BUFF SIZEOF_PAGE

/**
 * Set up kernel memory - virtual and physical.
 *
 * (1) kmalloc() works with a limited static allocatable memory
 * (2) Set up vma for kernel virtual memory space and video buffer memory
 *     - With kernel virtual memory space set up, kmalloc ask for additional
 *       allocatable space as soon as physical pages are scanned
 * (3) Scan physical pages
 *     - Start making use of space from kernel malloc. As soon as kmalloc runs
 *       out of allocatable memory, there must be at least one physical free
 *       page that can be given to kmalloc to continue with allocations. This
 *       makes the number of static allocatable pages in kmalloc critical!
 *       @see include/sys/memory/kmalloc.h
 *       TODO: THIS WILL NOT WORK COZ SELF REF PAGE TABLES ARE NOT SET UP YET!
 * (4) Initialize page table for all kernel page tables
 *
 * @param kernmem          Virtual address offset of kernel memory. Kernel spans
 *                         from kernmem to kernmem_off - p_kern_start + 
 *                         p_kern_end
 * @param p_kern_start     Physical start address of kernel
 * @param p_kern_end       Physical end address of kernel
 * @param p_vdo_buff_start Physical start address of video buffer. Size of video
 *                         buffer is SIZEOF_VDO_BUFF
 * @param modulep          physical memory address information
 * @return                 OK or ERROR
 */
int setup_kernel_memory(uint64_t kernmem, uint64_t p_kern_start,
                                uint64_t p_kern_end, uint64_t p_vdo_buff_start,
                                                            uint32_t *modulep);

#endif

