#include <defs.h>
#include "kstdio.h"

void start(void* modulep, void* physbase, void* physfree)
{
	// kernel starts here
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	volatile register char *rsp asm ("rsp");
	volatile register char *temp1, *temp2;
	loader_stack = (uint32_t*)rsp;
	rsp = &stack[INITIAL_STACK_SIZE];
	start(
		(char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase,
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;
	
        /* for(
                temp1 = "!!!!! end() returned !!!!!", temp2 = (char*)0xb8e00;
                *temp1;
                temp1 += 1, temp2 += 2
        ) *temp2 = *temp1; */ 

	clear_screen();
	puts((char *)"Hello\n");
    int z, a[10];
    z = printf("Testing string in printf: %s :and character: %c \n:-)\n", "hello guyz this is printf\n", 'x');
    printf("\nnum_char %d\n", z);
    z = printf("%d, %%, %d, %", 12323, 0);
    printf("\nnum_char %d\n", z);
    z = printf("%d is a negative num :P", -10);
    printf("\nnum_char %d\n", z);
    z = printf("\nHex value of %d is %x\n", 0x8234A678, 0x8234a678);
    printf("\nnum_char %d\n", z);
    z = printf("\n\n%p %d\n", &(a[0]), sizeof(int*));
    printf("\nnum_char %d\n", z);
    z = printf("\n\n%p %d\n", &(a[1]), sizeof(int*));
    printf("%d\n", &(a[1]) - &(a[0]));
    printf("\nnum_char %d\n", z);
	while(1) {
//	    puts((char *)"Hello\n");
//      foo("s", "hello guyz this is foo");
    };
}
