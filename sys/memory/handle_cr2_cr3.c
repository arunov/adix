#include <sys/memory/handle_cr2_cr3.h>
#include <defs.h>

void* get_cr2()
{
	uint64_t cr2_value;
	__asm__(
		"movq %%cr2, %%rax;"
		"movq %%rax, %0;"
		:"=g"(cr2_value)
		);
	return (void *)cr2_value;
}

struct str_cr3 get_cr3()
{
        uint64_t cr3_value;
        __asm__(
                "movq %%cr3, %%rax;"
                "movq %%rax, %0;"
                :"=g"(cr3_value)
                );
        return *((struct str_cr3 *)(&cr3_value));
}

void set_cr3(struct str_cr3 page_trans)
{
        uint64_t cr3_value=*((uint64_t *)(&page_trans));
        __asm__ volatile( 
                "movq %0, %%rax;"
                "movq %%rax, %%cr3;"
		:
		:"g"(cr3_value)
                );
}

