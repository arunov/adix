#include<defs.h>
#include<syscall.h>

void yield(){
	__syscall0(YIELD);
}

void exit(int status){
	__syscall1(EXIT,status);
}

void sleep(int sleep_interval){ //TODO
	__syscall1(SLEEP, sleep_interval);
}
