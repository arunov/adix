# Ref: http://www.osdever.net/bkerndev/Docs/isrs.htm

# Macro pushAllReg
.macro pushAllReg
pushq %rax
pushq %rbx
pushq %rcx
pushq %rdx
pushq %rdi
pushq %rsi
pushq %rsp
pushq %rbp
pushq %r8
pushq %r9
pushq %r10
pushq %r11
pushq %r12
pushq %r13
pushq %r14
pushq %r15
.endm

# Macro popAllReg
.macro popAllReg
popq %r15
popq %r14
popq %r13
popq %r12
popq %r11
popq %r10
popq %r9
popq %r8
popq %rbp
popq %rsp
popq %rsi
popq %rdi
popq %rdx
popq %rcx
popq %rbx
popq %rax
.endm


