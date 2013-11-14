#ifndef _SETUP_KERNEL_PGTBL_H
#define _SETUP_KERNEL_PGTBL_H
#include<sys/memory/handle_cr2_cr3.h>
void setup_kernel_pgtbl(void *kernmem, void *physbase, void *physfree);
struct str_cr3 create_kernel_pgtbl(void* kernmem, void* physbase, void* physfree); 
#endif
