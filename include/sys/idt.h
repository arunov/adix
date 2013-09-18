#ifndef _IDT_H
#define _IDT_H

#include <defs.h>
#include <sys/gdt.h>

/* adapted from Chris Stones, shovelos */

#define D_GATE   (0x000e0000000000)  /*** Size of gate 32 bits ***/

extern uint64_t idt[];

void reload_idt();

#endif
