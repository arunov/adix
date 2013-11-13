# Ref: http://www.osdever.net/bkerndev/Docs/irqs.htm
.include "sys/assembly_utils.s"
# Macro pushAllReg

# Service Routines (IRQs)
.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
.global irq16

#  0: Programmable Interval Timer 
irq0:
    cli
    pushq $0    # A normal ISR stub that pops a dummy error code to keep a
                   # uniform stack frame
    pushq $32
    jmp irq_common_stub

#  1: Keyboard
irq1:
    cli
    pushq $0
    pushq $33
    jmp irq_common_stub

#  2: Future Use
irq2:
    cli
    pushq $0
    pushq $34
    jmp irq_common_stub

#  3: Future Use
irq3:
    cli
    pushq $0
    pushq $35
    jmp irq_common_stub

#  4: Future Use
irq4:
    cli
    pushq $0
    pushq $36
    jmp irq_common_stub

#  5: Future Use
irq5:
    cli
    pushq $0
    pushq $37
    jmp irq_common_stub

#  6: Future Use
irq6:
    cli
    pushq $0
    pushq $38
    jmp irq_common_stub

#  7: Future Use
irq7:
    cli
    pushq $0
    pushq $39
    jmp irq_common_stub

#  8: Future Use
irq8:
    cli
    pushq $0
    pushq $40       
    jmp irq_common_stub

#  9: Future Use
irq9:
    cli
    pushq $0
    pushq $41
    jmp irq_common_stub

#  10: Future Use
irq10:
    cli
    pushq $0
    pushq $42
    jmp irq_common_stub

#  11: Future Use
irq11:
    cli
    pushq $0
    pushq $43
    jmp irq_common_stub

#  12: Future Use
irq12:
    cli
    pushq $0
    pushq $44
    jmp irq_common_stub

#  13: Future Use
irq13:
    cli
    pushq $0
    pushq $45
    jmp irq_common_stub

#  14: Future Use
irq14:
    cli
    pushq $0
    pushq $46
    jmp irq_common_stub

#  15: Future Use
irq15:
    cli
    pushq $0
    pushq $47
    jmp irq_common_stub

#  16: System call
irq16:
    cli
    pushq %rax
    pushq $48
    jmp irq_common_stub

# We call a C function in here. We need to let the assembler know
# that 'fault_handler' exists in another file
.extern irq_handler

# This is our common ISR stub. It saves the processor state, sets
# up for kernel mode segments, calls the C-level fault handler,
# and finally restores the stack frame.
irq_common_stub:
    #Save Current rdi
    pushq %rdi
    pushq %rsi

    #Index of the Interrupt_No in Stack
    movq %rsp, %rdi
    addq $0x10, %rdi
    movq %rsp, %rsi
    addq $0x18, %rsi

    #Save State
    pushAllReg

    #Save Segment
    movq %ds, %rdx
    pushq %rdx
    movq %es, %rdx
    pushq %rdx
    movq %fs, %rdx
    pushq %rdx
    movq %gs, %rdx
    pushq %rdx
    
    #Load Kernel Data Segment
    movabsq $0x10, %rax
    movq %rax, %ds
    movq %rax, %es
    movq %rax, %fs
    movq %rax, %gs

    call irq_handler

    #Retrieve Segment
    popq %rdx
    movq %rdx, %gs
    popq %rdx
    movq %rdx, %fs
    popq %rdx
    movq %rdx, %es
    popq %rdx
    movq %rdx, %ds

    #Retrieve State
    popAllReg
    popq %rsi	
    popq %rdi

    addq $0x10, %rsp   # Cleans up the pushed error code and pushed ISR number
    sti
    sti		#Why twice? Apparently schedule() disables other interrupts! 
    #WHY? No idea!! I have to move on, leaving it here for Arun to fix! :P
    iretq           # pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP!

