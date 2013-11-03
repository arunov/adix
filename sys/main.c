#include <defs.h>
#include <sys/kstdio.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/irq.h>
#include <sys/memory/phys_page_manager.h>
#include <sys/memory/handle_cr2_cr3.h>
#include <sys/memory/setup_kernel_pgtbl.h>
#include <sys/list.h>

#define INITIAL_STACK_SIZE 4096


char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;

void cooperative_schedule();
void test_print()
{
    char *str="Hello World\n";
    char c='A';
    int a=10;
	
    clear_screen();		
    //printf("~~Character: %c, Hex: %x, String: %s Address of String: %p\n", c,a,str,str);
    printf("~~Character: %c, Integer: %d, Hex: %x, String: %s Address of String: %p\n", c,a,a,str,str);
    printf("Address of kernmem: %p\n", &kernmem);
    printf("Offset od str: %x\n", ((uint64_t)str)-((uint64_t)&kernmem));
    printf("Content of cr2: %p\n",get_cr2());
    struct str_cr3 cr3= get_cr3();
    printf("Content of cr3: %x\n", *((uint64_t *)(&cr3)));
    cooperative_schedule();
/*
	mystruct myFirst = {
        	 .data = 10,
	         .myList = LIST_HEAD_INIT(myFirst.myList)
	};

	mystruct mySecond = {
        	 .data = 20,
		 .myList = LIST_HEAD_INIT(mySecond.myList)
	};

	LIST_HEAD(mylinkedlist);

	list_add( &myFirst.myList , &mylinkedlist ) ;
	list_add( &mySecond.myList , &mylinkedlist ) ;

	struct list_head* position;
	list_for_each( position , &mylinkedlist )
	{	
        	printf ("surfing the linked list next = %p and prev = %p\n" ,position->next, position->prev );
	}	

	mystruct  *datastructureptr = NULL ; 
	list_for_each_entry ( datastructureptr , &mylinkedlist, myList ){ 
		printf ("data  =  %d\n" , datastructureptr->data ); 
	}

	list_del(&myFirst.myList);
	int empty = list_empty(&mylinkedlist);
	if(empty==1){
		printf("deleted myFirst:  List is empty");
	}	

	list_del(&mySecond.myList);
	empty = list_empty(&mylinkedlist);
	if(empty==1){
		printf("deleted myFirst and mySecond:  List is empty");
	}	

	list_add_tail( &myFirst.myList , &mylinkedlist);
	list_add_tail( &mySecond.myList , &mylinkedlist ) ;
	list_for_each( position , &mylinkedlist )
	{
        	printf ("surfing the linked list next = %p and prev = %p\n" ,position->next, position->prev );
	}
*/	
}


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
	test_print();

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

	test_print();	

        /* for(
                temp1 = "!!!!! end() returned !!!!!", temp2 = (char*)0xb8e00;
                *temp1;
                temp1 += 1, temp2 += 2
        ) *temp2 = *temp1; */ 

    /*
	puts((char *)"Hello\n");
    int z, a[10];
    z = printf("Testing string in printf: %s :and character: %c \n:-)\n", "hello guyz this is printf\n", 'x');
    printf("\nnum_char %d\n", z);
    z = printf("Integer %d, %%, %d, %", 12323, 0);
    printf("\nnum_char %d\n", z);
    z = printf("Negative Integer %d is a negative num :P", -10);
    printf("\nnum_char %d\n", z);
    z = printf("\nHex value of %d is %x\n", 0x8234A678, 0x8234a678);
    printf("\nnum_char %d\n", z);
    z = printf("\n\nPointer a[0] %p %d\n", &(a[0]), sizeof(int*));
    printf("\nnum_char %d\n", z);
    z = printf("\n\nPointer a[1] %p %d\n", &(a[1]), sizeof(int*));
    printf("\nnum_char %d\n", z);
    z = printf("\n\nPointer z %p %d\n", &z, sizeof(int*));
    int *t1 = &a[0];
    int *t2 = &a[1];
    printf("Difference in address %d\n", (t2 - t1));
    printf("\nnum_char %d\n", z);
    volatile int qqq = 0;

    printf("%d", 1/qqq);*/

    //int a=10;
    //printf("Address of a:%p\n", &a);

    //clear_screen();
    test_print();

	while(1) {
    };
}
