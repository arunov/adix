#include "sys/idt.h"
#include "sys/isr.h"
#include "kstdio.h"
#include "kstring.h"

void idt_set_gate(int num, uint64_t isr_addr) {
	idt[num].flags = IDT_P | IDT_DPL0 | TYPE_INTERRUPT_GATE;
	idt[num].seg_sel = 0x08;
	idt[num].offset_low    = ((isr_addr & 0x000000000000ffff)); 
    idt[num].offset_middle = ((isr_addr & 0x00000000ffff0000) >> 16);
	idt[num].offset_high   = ((isr_addr & 0xffffffff00000000) >> 32);
#if DEBUG
    printf("idt entry %d: idt num: %p idt+num+64: %p\n", num, idt[num], *((char *)(&idt[num])+8));
    printf("%d - function addr: %p\n", num, isr_addr);
#endif
}

void isrs_install()
{
    /* Clear out the entire IDT, initializing it to zeros */
    memset(&idt, 0, sizeof(struct idt_t) * 256);

    idt_set_gate(0, (uint64_t)isr0);
    idt_set_gate(1, (uint64_t)isr1);
    idt_set_gate(2, (uint64_t)isr2);
    idt_set_gate(3, (uint64_t)isr3);
    idt_set_gate(4, (uint64_t)isr4);
    idt_set_gate(5, (uint64_t)isr5);
    idt_set_gate(6, (uint64_t)isr6);
    idt_set_gate(7, (uint64_t)isr7);
    idt_set_gate(8, (uint64_t)isr8);
    idt_set_gate(9, (uint64_t)isr9);
    idt_set_gate(10, (uint64_t)isr10);
    idt_set_gate(11, (uint64_t)isr11);
    idt_set_gate(12, (uint64_t)isr12);
    idt_set_gate(13, (uint64_t)isr13);
    idt_set_gate(14, (uint64_t)isr14);
    idt_set_gate(15, (uint64_t)isr15);
    idt_set_gate(16, (uint64_t)isr16);
    idt_set_gate(17, (uint64_t)isr17);
    idt_set_gate(18, (uint64_t)isr18);
    idt_set_gate(19, (uint64_t)isr19);
    idt_set_gate(20, (uint64_t)isr20);
    idt_set_gate(21, (uint64_t)isr21);
    idt_set_gate(22, (uint64_t)isr22);
    idt_set_gate(23, (uint64_t)isr23);
    idt_set_gate(24, (uint64_t)isr24);
    idt_set_gate(25, (uint64_t)isr25);
    idt_set_gate(26, (uint64_t)isr26);
    idt_set_gate(27, (uint64_t)isr27);
    idt_set_gate(28, (uint64_t)isr28);
    idt_set_gate(29, (uint64_t)isr29);
    idt_set_gate(30, (uint64_t)isr30);
    idt_set_gate(31, (uint64_t)isr31);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *exception_messages[] =
{
    "Division By Zero Exception",               //  0
    "Debug Exception",                          //  1
    "Non Maskable Interrupt Exception",         //  2
    "Breakpoint Exception",                     //  3
    "Into Detected Overflow Exception",         //  4
    "Out of Bounds Exception",                  //  5
    "Invalid Opcode Exception",                 //  6
    "No Coprocessor Exception",                 //  7
    "Double Fault Exception",                   //  8
    "Coprocessor Segment Overrun Exception",    //  9
    "Bad TSS Exception",                        // 10
    "Segment Not Present Exception",            // 11
    "Stack Fault Exception",                    // 12
    "General Protection Fault Exception",       // 13
    "Page Fault Exception",                     // 14
    "Unknown Interrupt Exception",              // 15
    "Coprocessor Fault Exception",              // 16
    "Alignment Check Exception (486+)",         // 17
    "Machine Check Exception (Pentium/586+)",   // 18
    "Reserved Exceptions",                      // 19
    "Reserved Exceptions",                      // 20
    "Reserved Exceptions",                      // 21
    "Reserved Exceptions",                      // 22
    "Reserved Exceptions",                      // 23
    "Reserved Exceptions",                      // 24
    "Reserved Exceptions",                      // 25
    "Reserved Exceptions",                      // 26
    "Reserved Exceptions",                      // 27
    "Reserved Exceptions",                      // 28
    "Reserved Exceptions",                      // 29
    "Reserved Exceptions",                      // 30
    "Reserved Exceptions"                       // 31
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(uint64_t *r)
{
    /* Is this a fault whose number is from 0 to 31? */
    if (*r < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
        puts(exception_messages[*r]);
        puts(" Exception. System Halted!\n");
        for (;;);
    }
    for (;;);
}

