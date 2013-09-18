#include "sys/idt.h"
#include "sys/isr.h"
#include "kstdio.h"

void idt_set_gate(int num, unsigned int isr_addr) {
    idt[num] = P | DPL0 | D_GATE | 0x80000 | ((uint64_t)isr_addr & 0xffff) |
        ((uint64_t)((uint64_t)isr_addr & 0xffff0000) << 32);
#if DEBUG
    printf("idt entry %d: %p\n", num, idt[num]);
    printf("31 - function addr: %p\n", isr31);
#endif
}

void isrs_install()
{
    idt_set_gate(0, (unsigned int)isr0);
    idt_set_gate(1, (unsigned int)isr1);
    idt_set_gate(2, (unsigned int)isr2);
    idt_set_gate(3, (unsigned int)isr3);
    idt_set_gate(4, (unsigned int)isr4);
    idt_set_gate(5, (unsigned int)isr5);
    idt_set_gate(6, (unsigned int)isr6);
    idt_set_gate(7, (unsigned int)isr7);
    idt_set_gate(8, (unsigned int)isr8);
    idt_set_gate(9, (unsigned int)isr9);
    idt_set_gate(10, (unsigned int)isr10);
    idt_set_gate(11, (unsigned int)isr11);
    idt_set_gate(12, (unsigned int)isr12);
    idt_set_gate(13, (unsigned int)isr13);
    idt_set_gate(14, (unsigned int)isr14);
    idt_set_gate(15, (unsigned int)isr15);
    idt_set_gate(16, (unsigned int)isr16);
    idt_set_gate(17, (unsigned int)isr17);
    idt_set_gate(18, (unsigned int)isr18);
    idt_set_gate(19, (unsigned int)isr19);
    idt_set_gate(20, (unsigned int)isr20);
    idt_set_gate(21, (unsigned int)isr21);
    idt_set_gate(22, (unsigned int)isr22);
    idt_set_gate(23, (unsigned int)isr23);
    idt_set_gate(24, (unsigned int)isr24);
    idt_set_gate(25, (unsigned int)isr25);
    idt_set_gate(26, (unsigned int)isr26);
    idt_set_gate(27, (unsigned int)isr27);
    idt_set_gate(28, (unsigned int)isr28);
    idt_set_gate(29, (unsigned int)isr29);
    idt_set_gate(30, (unsigned int)isr30);
    idt_set_gate(31, (unsigned int)isr31);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *exception_messages[] =
{
    "Division By Zero Exception",
    "Debug Exception",
    "Non Maskable Interrupt Exception",
    "Breakpoint Exception",
    "Into Detected Overflow Exception",
    "Out of Bounds Exception",
    "Invalid Opcode Exception",
    "No Coprocessor Exception",
    "Double Fault Exception",
    "Coprocessor Segment Overrun Exception",
    "Bad TSS Exception",
    "Segment Not Present Exception",
    "Stack Fault Exception",
    "General Protection Fault Exception",
    "Page Fault Exception",
    "Unknown Interrupt Exception",
    "Coprocessor Fault Exception",
    "Alignment Check Exception (486+)",
    "Machine Check Exception (Pentium/586+)",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions",
    "Reserved Exceptions"
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void fault_handler(int *r)
{
    /* Is this a fault whose number is from 0 to 31? */
#if DEBUG
    printf("fault_handler: %d\n", *r);
#endif
    for (;;);
    if (*r < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
        puts(exception_messages[*r]);
        puts(" Exception. System Halted!\n");
        for (;;);
    }
}

