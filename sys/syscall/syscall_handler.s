.include "sys/assembly_utils.s"
.global sys_call_handler
.global _sys_yield
.global _sys_printf 
.global _sys_exit
.global _sys_open
.global _sys_read
.global _sys_write
.global _sys_lseek
.global _sys_close
.global _sys_opendir
.global _sys_readdir
.global _sys_closedir
.global _sys_sleep
.global _sys_clrscr
.global _sys_fork

.extern sys_call_table
.extern sys_yield
.extern sys_exit
.extern sys_open
.extern sys_read_stub
.extern sys_write_stub
.extern sys_lseek
.extern sys_close
.extern sys_opendir
.extern sys_readdir
.extern sys_closedir
.extern sys_sleep
.extern sys_fork
.extern printf
.extern set_tss
.extern clear_screen
sys_call_handler:
	pushAllSysReg
	movq $0x00,%rbx
	movq sys_call_table(%rbx,%rax,0x8),%rbx
	call *%rbx 
	popAllSysReg
	iretq

_sys_yield:
	call sys_yield
	#Setup TSS
	movq %rsp,%rdi
	addq $0xB0,%rdi
	call set_tss
	#Load new valure for tss
	movq $0x28, %rdi
	ltr %di
	retq

_sys_printf:
	call printf
	retq

_sys_exit:
	call sys_exit
	retq

_sys_open:
	call sys_open
	retq

_sys_write:
	call sys_write_stub
	retq

_sys_read:
	call sys_read_stub
	retq

_sys_lseek:
	call sys_lseek
	retq

_sys_close:
	call sys_close
	retq

_sys_opendir:
	call sys_opendir
	retq

_sys_readdir:
	call sys_readdir
	retq

_sys_closedir:
	call sys_closedir
	retq
	
_sys_sleep:
	call sys_sleep
	retq

_sys_clrscr:
	call clear_screen
	retq

_sys_fork:
	call sys_fork
    pushq %rax
	#Setup TSS
	movq %rsp,%rdi
	addq $0xB0,%rdi
	call set_tss
	#Load new valure for tss
	movq $0x28, %rdi
	ltr %di
    popq %rax
	retq

