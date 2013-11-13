
.global _jump_to_ring3
.extern set_tss_rsp

_jump_to_ring3:
	#RPL (3)| DS_segment_selector(32 - 0x20)
	cli
	movq $0x23,%rax
	movq %rax, %ds
	#movq %rax, %ss
	movq %rax, %es
	movq %rax, %fs
	movq %rax, %gs
	#Prepare TSS
	pushq %rdi
	movq %rsp, %rdi
	addq $0x08, %rdi
	call set_tss_rsp
	
	movq $0x28, %rdi
	ltr %di #Load ltr with offset
	popq %rdi

	pushq $0x23 #Push SS
	pushq %rdi #Push rsp
	pushfq
	#RPL (3) | CS_segment_selector (24 - 0x18)
	pushq $0x1b
	pushq %rsi #Push rip
	iretq
