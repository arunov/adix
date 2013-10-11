.text

######
# load a new IDT
#  parameter 1: address of idtr
.global _x86_64_asm_lidt
_x86_64_asm_lidt:

	lidt (%rdi)
	retq

