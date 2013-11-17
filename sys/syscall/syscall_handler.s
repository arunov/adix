.include "sys/assembly_utils.s"
.global sys_call_handler
.global _sys_yield
.global _sys_printf 
.global _sys_exit

.extern sys_call_table
.extern sys_yield
.extern sys_exit

sys_call_handler:
	pushAllReg
	movq $0x00,%rbx
	movq sys_call_table(%rbx,%rax,0x8),%rbx
	call *%rbx  
	popAllReg
	iretq

_sys_yield:
	call sys_yield
	retq

_sys_printf:
	retq

_sys_exit:
	call sys_exit
	retq
