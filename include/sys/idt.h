#ifndef _IDT_H
#define _IDT_H

#include <defs.h>
#include <sys/gdt.h>

/* adapted from Chris Stones, shovelos */
/* and 64-ia-32-architectures-software-developer-vol-3a-part-1-manual */

struct idt_t {
        uint16_t offset_low;
	uint16_t seg_sel;
	unsigned char always0_ist;
	unsigned char flags;
	uint16_t offset_middle;
	uint32_t offset_high;
	uint32_t reserved; 
}__attribute__((packed));


#define IDT_DPL0          (0x00)  /*** descriptor privilege level 0 ***/
#define IDT_DPL1          (0x20)  /*** descriptor privilege level 1 ***/
#define IDT_DPL2          (0x40)  /*** descriptor privilege level 2 ***/
#define IDT_DPL3          (0x60)  /*** descriptor privilege level 3 ***/
#define IDT_P             (0x80)  /*** present ***/

#define TYPE_INTERRUPT_GATE  (0x0e)  

extern struct idt_t idt[];

void reload_idt();

#endif
