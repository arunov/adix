#include "sys/idt.h"
#include "sys/isr.h"
#include "sys/irq.h"
#include "kstdio.h"
#include "kstring.h"

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(void))
{
    irq_routines[irq] = handler;
}

/* This clears the handler for a given IRQ */
void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void)
{
    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);
    outportb(0x21, 0x0);
    outportb(0xA1, 0x0);
}

void irq_install()
{
    irq_remap();
    idt_set_gate(32, (uint64_t)irq0);
    idt_set_gate(33, (uint64_t)irq1);
    idt_set_gate(34, (uint64_t)irq2);
    idt_set_gate(35, (uint64_t)irq3);
    idt_set_gate(36, (uint64_t)irq4);
    idt_set_gate(37, (uint64_t)irq5);
    idt_set_gate(38, (uint64_t)irq6);
    idt_set_gate(39, (uint64_t)irq7);
    idt_set_gate(40, (uint64_t)irq8);
    idt_set_gate(41, (uint64_t)irq9);
    idt_set_gate(42, (uint64_t)irq10);
    idt_set_gate(43, (uint64_t)irq11);
    idt_set_gate(44, (uint64_t)irq12);
    idt_set_gate(45, (uint64_t)irq13);
    idt_set_gate(46, (uint64_t)irq14);
    idt_set_gate(47, (uint64_t)irq15);
}

/* This is a simple string array. It contains the message that
*  corresponds to each and every exception. We get the correct
*  message by accessing like:
*  exception_message[interrupt_number] */
char *irq_messages[] =
{
    "Programmable Interval Timer\n",
    "Keyboard\n",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
    "Future Use",
};

/* All of our Exception handling Interrupt Service Routines will
*  point to this function. This will tell us what exception has
*  happened! Right now, we simply halt the system by hitting an
*  endless loop. All ISRs disable interrupts while they are being
*  serviced as a 'locking' mechanism to prevent an IRQ from
*  happening and messing up kernel data structures */
void irq_handler(uint64_t *r)
{
    /* Is this a fault whose number is from 32 to 47? */
#if DEBUG
    //printf("Interrupt No: %p\n", *r);
#endif
    if (*r > 31 && *r < 48)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */

        //puts(irq_messages[*r-32]); 

        /* This is a blank function pointer */
        void (*handler)(void);

        /* Find out if we have a custom handler to run for this
        *  IRQ, and then finally, run it */
        handler = irq_routines[*r - 32];
        if (handler)
        {
            handler();
        }

        /* If the IDT entry that was invoked was greater than 40
        *  (meaning IRQ8 - 15), then we need to send an EOI to
        *  the slave controller */
        if (*r >= 40)
        {
            outportb(0xA0, 0x20);
        }

        /* In either case, we need to send an EOI to the master
        *  interrupt controller too */
        outportb(0x20, 0x20);

        if(!handler) {
	    printf("IRQ Received: %d\n",*r);
            //puts(" IRQ. System Halted!\n");
            //for (;;);
        }
    }
}

