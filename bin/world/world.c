#include <stdlib.h>
#include <syscall.h>
int main(int argc, char* argv[], char* envp[]) {
	int wcounter =0 ;
	while(wcounter++ < 5){
		uprintf("World!\n");
		yield();
	}
	return 0;
}
