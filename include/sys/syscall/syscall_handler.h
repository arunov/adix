#ifndef SYSCALL_HANDLER_H
#define SYSCALL_HANDLER_H

#define NUM_SYS_CALLS 220

void _sys_yield();
void _sys_printf();
void _sys_exit();
void _sys_open();
void _sys_read();
void _sys_write();
void _sys_lseek();
void _sys_close();
void _sys_opendir();
void _sys_readdir();
void _sys_closedir();
void _sys_sleep();
void sys_call_handler();
int64_t sys_read_stub(int fd, void *buf, uint64_t count);
int sys_lseek_stub(int fd, off64_t offset, int whence);
int sys_close_stub(int fd);
int sys_opendir_stub(const char *pathname);
struct posix_header_ustar* sys_readdir_stub(int fd, uint64_t ret);
int sys_closedir_stub(int fd);
void _sys_clrscr();
#endif
