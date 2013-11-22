#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <defs.h>
#include <sys/memory/page_table_helper.h>
#include <sys/list.h>
#include <sys/memory/mm_struct.h>

/**
 * Allocate bytes of memory for kernel
 * @param bytes number of bytes to be allocated 
 * @return      virtual memory pointer to start of allocated memory
 */
void* kmalloc(uint64_t bytes);

/**
 * Free memory pointed by ptr
 * @param ptr virtual memory pointer to start of memory that should be freed
 */
void kfree(void *ptr);

/**
 * Call this to indicate that device memory is set up. If kmalloc runs out of
 * memory, it will allocate more pages only if device memory is set up
 * completely. Otherwise KERNEL PANIC!
 */
void kmDeviceMemorySetUpDone();

/******************************************************************************/

/* Number of statically allocated pages for kmalloc */
/* NOTE: >= 1 */
#define KM_STATIC_PAGES 10240

/* Bytes of statically allocated pages for kmalloc */
#define KM_STATIC_SIZE SIZEOF_PAGE * KM_STATIC_PAGES

/* Number of additional pages when kmalloc runs out of allocatable pages */
#define KM_PAGES_ADD 10

#endif

