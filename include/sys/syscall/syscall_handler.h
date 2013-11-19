#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#define NUM_SYS_CALLS 15

void _sys_yield();
void _sys_printf();
void _sys_exit();
void _sys_open();
void _sys_read();
void _sys_lseek();
void _sys_close();
void _sys_opendir();
void _sys_readdir();
void _sys_closedir();
void sys_call_handler();
#endif
