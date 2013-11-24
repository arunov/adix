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
    setup_kernel_memory((uint64_t)&kernmem, (uint64_t)physbase, (uint64_t)physfree, VIDEO_MEMORY_ADDRESS, modulep);

    add_vma(&get_kernel_mm()->mmap, 0x8000, 0x12000, PAGE_TRANS_READ_WRITE, MAP_ANONYMOUS);
    uint64_t phys1 = alloc_phys_pages(1);
    update_curr_page_table(phys1, 0x10000, 0);

    *((int*)0x10000) = 5;
    *((int*)0x10004) = 6;

    printf("hi :) %d %d\n", *((int*)0x10000), *((int*)0x10004));
    printf("cr4: %p\n", get_cr4());

    uint64_t phys;
    char *x = (char*)v_alloc_page_get_phys(&phys, PAGE_TRANS_READ_WRITE);
    *x = 'a';
    *(x+1) = 'b';
    *(x+2) = 'c';
    *(x+3) = '\0';
    uint64_t perm;
    printf("phys addr expected = %p, virt addr = %p, content = %s, physical addr actual = %p", phys, x, x, virt2phys_selfref((uint64_t)x, &perm));
    printf(", perm = %p\n\n", perm);
    v_free_page((uint64_t) x);

    *x = 'x';
    printf("phys addr expected = %p, virt addr = %p, content = %s, physical addr actual = %p", phys, x, x, virt2phys_selfref((uint64_t)x, &perm));
    printf(", perm = %p\n\n", perm);

#if 0
    printf("physical address of kernmem %p is %p\n", &kernmem, virt2phys_selfref((uint64_t)&kernmem));

	printf("Page tables successfully setup\n");

	printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
	
	int fd = sys_open("aladdin.txt");
	printf("\nFd returned after syscall %d",fd);
#endif

	uint64_t execret = exec("bin/hello");
	printf("execret %p", execret);
	
/*    
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
