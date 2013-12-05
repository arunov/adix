.global _jump_to_ring3

_jump_to_ring3:
	#RPL (3)| DS_segment_selector(32 - 0x20)
	movq $0x23,%rax
	movq %rax, %ds
	#movq %rax, %ss
	movq %rax, %es
	movq %rax, %fs
	movq %rax, %gs
	#Prepare TSS
	
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8

	popq %r8
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi
	
	pushq $0x23 #Push SS
	pushq %rdi #Push rsp
	sti
	pushfq
	cli
	#RPL (3) | CS_segment_selector (24 - 0x18)
	pushq $0x1b
	pushq %rsi #Push rip
	#prepare parameters for the user process
	movq %rdx, %rdi
	movq %rcx, %rsi
	movq %r8, %rdx
	iretq
