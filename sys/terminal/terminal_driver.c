#include <defs.h>
#include <sys/kstdio.h>
#include <sys/terminal/terminal_driver.h>
#include <sys/filesystems/file_structures.h>
#include <sys/scheduler/scheduler.h>
#include <sys/kstring.h>
#define FLUSH_CHAR '\n'

char terminal_buffer[1024];
uint64_t buffer_len = 0; 

int terminal_open(const char* pathname){
//	return add_to_process_file_table(getCurrentTask, pft);
	return 0;
}

uint64_t terminal_read(int fd, void *buf, uint64_t count){
	sys_sleep(TERMINAL_TYPE);
	memcpy(buf, terminal_buffer, count);
	return count;//TODO: Read MAX(count, bufferlen)
}

uint64_t terminal_write(int fd, void *buf, uint64_t count){
	((char*)(buf))[count] = '\0';
	return printf(buf);
}
static void terminal_flush(){
	buffer_len = 0;
	sys_wakeup(TERMINAL_TYPE);
}

void terminal_enqueue(char input_char){
	printf("%c",input_char);
	if(input_char == FLUSH_CHAR){
		terminal_buffer[buffer_len] = '\0';
		terminal_flush();
		return;
	}
	terminal_buffer[buffer_len++] = input_char;
	//TODO:handle delete etc

}


int terminal_close(int fd){
	return 0;
}


