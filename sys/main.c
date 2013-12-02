#include <defs.h>
#include <sys/kstdio.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/handle_cr.h>
#include <sys/memory/setup_kernel_pgtbl.h>
#include <sys/list.h>
#include <sys/parser/parsetarfs.h>
#include <sys/parser/tarfs.h>
#include <sys/parser/parseelf.h>
#include <sys/parser/exec.h>
#include <sys/memory/mm_struct.h>
#include <sys/memory/setup_kernel_memory.h>
#include <sys/memory/free_phys_pages.h>
#include <sys/ahci/ahci.h>
#include <sys/memory/phys_page_manager2.h>

#define INITIAL_STACK_SIZE 4096
#define SET_NXE 0x800ULL

char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
struct tss_t tss;


void cooperative_schedule(void *, void *);

struct phys_page_manager phys_page_mngr_obj;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
/*
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
        
*/	
	/* Free list and Page Tables */
	//phys_page_manager_init(&phys_page_mngr_obj, modulep, physbase, physfree);
    //    finish_scan(&phys_page_mngr_obj);
        setup_kernel_memory((uint64_t)&kernmem, (uint64_t)physbase, (uint64_t)physfree, VIDEO_MEMORY_ADDRESS, modulep);
	//setup_kernel_pgtbl(&kernmem, physbase, physfree);
/*
    uint64_t phys;
    char *x = (char*)v_alloc_page_get_phys(&phys, PAGE_TRANS_READ_WRITE);
    *x = 'a';
    *(x+1) = 'b';
    *(x+2) = 'c';
    *(x+3) = '\0';
    printf("phys addr expected = %p, virt addr = %p, content = %s, physical addr actual = %p\n\n", phys, x, x, virt2phys_selfref((uint64_t)x));

    v_free_page((uint64_t) x);
#if 0
    printf("physical address of kernmem %p is %p\n", &kernmem, virt2phys_selfref((uint64_t)&kernmem));

	printf("Page tables successfully setup\n");

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	
	int fd = sys_open("aladdin.txt");
	printf("\nFd returned after syscall %d",fd);
#endif
*/
/*
	uint64_t execret = exec("bin/hello");
	printf("execret %p", execret);
*/	
	//check asci
/*	int ahci_ret = setup_ahci();
	printf("asci_ret: %d\n", ahci_ret);
	int ahci_command_ret= ahcicommands();
	printf("ahci_command_ret: %d\n",ahci_command_ret);
  */ /* 
    // Check do_mmap 
    int fd = open("aladdin.txt");
    struct mm_struct *mm = new_mm();
    do_mmap(&(mm->mmap), fd, 0, 0x1000, 100, PAGE_TRANS_READ_WRITE | PAGE_TRANS_USER_SUPERVISOR);
    printf("Contents of mmapped file: %s", 0x1000UL);
*/	
    	cooperative_schedule(&kernmem,physfree);
	// kernel starts here
	

	while(1);
}

void boot(void)
{
	/*
	uint64_t efer; 
	__asm__ volatile(
		"movq %%EFER, %0;"
		:"=g"(efer)
		:
	);
	efer = efer | SET_NXE;
	__asm__ volatile(
		"movq %0, %%efer;"
		:
		:"g"(efer)
	);
	*/

	// note: function changes rsp, local stack variables can't be practically used
	register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		"cli" //Disable interrupts till first process inialization
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
