#include<defs.h>
#include<syscall.h>

void yield(){
	__syscall0(YIELD);
}

void exit(int status){
	__syscall1(EXIT,status);
}
