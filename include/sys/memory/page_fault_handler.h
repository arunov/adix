#ifndef _PAGE_FAULT_HANDLER_H
#define _PAGE_FAULT_HANDLER_H

#include <defs.h>

/**
 * When page fault occurs:
 * (1) Check address that caused fault
 * (2) Check if memory area exists for the address
 * (3) If yes, add a physical page for the location with required protection
 * (4) In case of file backed vma, load file contents into page
 * @param err_code page-fault error code
 * @return         OK or ERROR
 */
int page_fault_handler(uint64_t err_code);


/*----------------------------------------------------------------------------*/
/* Page fault error code */

/* Bit 0: 0 - Page not present 1 - Page protection violation */
#define PF_ERROR_CODE_P   0x1UL

/* Bit 1: 0 - Memory read      1 - Memory write */
#define PF_ERROR_CODE_W   0x2UL

/* Bit 2: 0 - Supervisor mode  1 - User mode */
#define PF_ERROR_CODE_U   0x4UL

/*-------- Kinda not used --------*/
/* Bit 3: RSV */
#define PF_ERROR_CODE_RSV 0x8UL

/* Bit 4: ID */
#define PF_ERROR_CODE_ID  0x16UL
/*--------------------------------*/

/*----------------------------------------------------------------------------*/

#endif

