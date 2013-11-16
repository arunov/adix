#include <defs.h>
#include <sys/kstdio.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/handle_cr2_cr3.h>
#include <sys/memory/setup_kernel_pgtbl.h>
#include <sys/list.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/tarfs.h>

#define INITIAL_STACK_SIZE 4096


char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;


void cooperative_schedule(void *, void *);

struct phys_page_manager phys_page_mngr_obj;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;

	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 && smap->length != 0) {
			printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
			printf("No of pages: %d\n", (smap->length/PG_SZ));
		}
	}
	printf("Number of pages scanned: %d\n", phys_page_mngr_obj.n_nodes);
	printf("Physbase: %p, Physfree: %p\n", physbase, physfree);
	printf("Total pages for the current kernel: %d\n", (((uint64_t)physfree)-((uint64_t)physbase))/PG_SZ);

        /* Free list and Page Tables */
	phys_page_manager_init(&phys_page_mngr_obj, modulep, physbase, physfree);
        finish_scan(&phys_page_mngr_obj);
	setup_kernel_pgtbl(&kernmem, physbase, physfree);

	printf("Page tables successfully setup\n");
    	cooperative_schedule(&kernmem,physfree);

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

	// kernel starts here
	while(1);
}

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);

	global_video_vaddr = (void *)VIDEO_MEMORY_ADDRESS;

	reload_gdt();
	reload_idt();
	setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;


	while(1) {
    };
}
