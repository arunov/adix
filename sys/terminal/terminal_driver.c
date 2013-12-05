#include <defs.h>
#include <sys/kstdio.h>
#include <sys/terminal/terminal_driver.h>
#include <sys/filesystems/file_structures.h>
#include <sys/scheduler/scheduler.h>
#include <string.h>

char terminal_buffer[1024];
uint64_t buffer_len = 0; 

struct operation_pointers terminal_ops = {
	terminal_open,
	terminal_read,
	terminal_write,
	NULL,
	terminal_close,
	NULL,
	NULL,
	NULL
};
int terminal_open(const char* pathname){
//	return add_to_process_file_table(getCurrentTask, pft);
	return 0;
}

int64_t terminal_read(int fd, void *buf, uint64_t count){
	sys_sleep(TERMINAL_TYPE);
	memcpy(buf, terminal_buffer, buffer_len);
	count = buffer_len;
	buffer_len = 0;
	return count;
}

int64_t terminal_write(int fd, void *buf, uint64_t count){
	((char*)(buf))[count] = '\0';
	return printf(buf);
}
static void terminal_flush(){
	sys_wakeup(TERMINAL_TYPE);//TODO: WAkeup foreground process
}

void terminal_enqueue(char input_char){
	printf("%c",input_char);
	if(input_char == '\b'){
		terminal_buffer[buffer_len] = '\0';
		buffer_len--;
		terminal_buffer[buffer_len] = '\0';
		return;
	}
	if(input_char == FLUSH_CHAR){
		terminal_buffer[buffer_len] = '\0';
		terminal_flush();
		return;
	}
	if(buffer_len == 0 && input_char == ' '){
	/* Ignore leading backspaces */
		return;
	}
	terminal_buffer[buffer_len++] = input_char;
}


int terminal_close(int fd){
	return 0;
}

struct operation_pointers* get_terminal_ops(){
	return &terminal_ops;
}
