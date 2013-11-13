#ifndef _RING_SWITCH_H
#define _RING_SWITCH_H
#include<defs.h>

/* Initial switch to ring 3, calls _jum_to_ring3 */
void jump_to_ring3();

/* Prepares stack to jump to user space and does an iretq*/
void _jump_to_ring3(uint64_t rsp, uint64_t rip);

#endif
