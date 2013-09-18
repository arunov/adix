#include <sys/idt.h>
#include "kstring.h"
#include "sys/isr.h"

extern void isr0();

/* adapted from Chris Stones, shovelos */

#define MAX_IDT 256

uint64_t idt[MAX_IDT];

struct idtr_t {
	uint16_t size;
	uint64_t addr;
}__attribute__((packed));

static struct idtr_t idtr = {
	sizeof(idt),
	(uint64_t)idt,
};

void _x86_64_asm_lidt(struct idtr_t* idtr);
void reload_idt() {
    isrs_install();
	_x86_64_asm_lidt(&idtr);
}
