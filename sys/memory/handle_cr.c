#include <sys/memory/handle_cr.h>
#include <defs.h>

struct str_cr3 get_default_cr3() {

	struct str_cr3 cr3;
	cr3.reserved_low = 0x0;
	//cr3.PWT = 0x01;
	//cr3.PCD = 0x01;
	cr3.reserved_high = 0x0;
	cr3.reserved_MBZ = 0x0;

	return cr3;
}

uint64_t get_cr0()
{
	uint64_t cr0_value;
	__asm__ volatile(
		"movq %%cr0, %%rax;"
		"movq %%rax, %0;"
		:"=g"(cr0_value)
		);
	return cr0_value;
}

uint64_t get_cr4()
{
	uint64_t cr4_value;
	__asm__ volatile(
		"movq %%cr4, %%rax;"
		"movq %%rax, %0;"
		:"=g"(cr4_value)
		);
	return cr4_value;
}

void* get_cr2()
{
	uint64_t cr2_value;
	__asm__ volatile(
		"movq %%cr2, %%rax;"
		"movq %%rax, %0;"
		:"=g"(cr2_value)
		);
	return (void *)cr2_value;
}

struct str_cr3 get_cr3()
{
        uint64_t cr3_value;
        __asm__ volatile(
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

void set_cr0(uint64_t val)
{
        __asm__ volatile( 
                "movq %0, %%rax;"
                "movq %%rax, %%cr0;"
		:
		:"g"(val)
                );
}

