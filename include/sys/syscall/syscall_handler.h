#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#define NUM_SYS_CALLS 10

void _sys_yield();
void _sys_printf();
void _sys_exit();
void sys_call_handler();
#endif
