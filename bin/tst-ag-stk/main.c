#include <stdio.h>
#include <syscall.h>

void func(char x[]) {
}

int main() {
    //Testing auto-growing stack
    //GROWDOWN of stack is allowed in page units;
    //Declared array of size 15000 bytes (char). x[0] is far from current
    //stack page, x[15000] is near to current stack page
    char x[15000];
    //Stack growth by 1 page works fine for ulimit -s 8192 or greater
    x[10000] = 'a';
    //Stack growth by 2 pages works fails for ulimit -s 12288 or lesser
    x[0] = 'b';
    func(x);
}

