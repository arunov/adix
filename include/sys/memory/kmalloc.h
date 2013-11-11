#ifndef _KMALLOC_H
#define _KMALLOC_H

#include <defs.h>
#include <sys/memory/page_table_helper.h>
#include <sys/list.h>

/* Alignment options for kmalloc */
#define KM_ALIGN_PAGE  0x1000ULL
#define KM_ALIGN_8BYTE 0x0008ULL
#define KM_ALIGN_4BYTE 0x0004ULL
#define KM_ALIGN_BYTE  0x0001ULL

#define KM_

/**
 * Allocate bytes of memory for kernel
 * @param bytes number of bytes to be allocated 
 * @return      virtual memory pointer to start of allocated memory
 */
#define kmalloc(bytes) kmalloc_align(bytes, KM_ALIGN_BYTE);

/**
 * Allocate bytes of memory for kernel that is aligned
 * @param bytes number of bytes to be allocated
 * @param align alignment type: KMALLOC_ALIGN_PAGE for page aligned,
 *              KM_ALIGN_8BYTE for 8 byte aligned, KM_ALIGN_4BYTE for 4 byte
 *              aligned
 *                              
 * @return      virtual memory pointer to start of allocated memory
 */
void* kmalloc_align(uint64_t bytes, uint64_t align);

/**
 * Free memory pointed by ptr
 * @param ptr virtual memory pointer to start of memory that should be freed
 */
void kfree(void *ptr);

/******************************************************************************/

/* Number of statically allocated pages for kmalloc */
/* NOTE: >= 1 */
#define KM_STATIC_PAGES 10

/* Bytes of statically allocated pages for kmalloc */
#define KM_STATIC_SIZE SIZEOF_PAGE * KM_STATIC_PAGES

#endif

