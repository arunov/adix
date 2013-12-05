#include <stdio.h>
#include <syscall.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fd = -1;

    if(argc == 2) {
        int num = atoi(argv[1]), i;
        for(i = 0; i < num; i ++) {

            int ret = open("aladdin.txt");

            if(-1 == ret) {
                break;
            }

            fd = ret;
        }
        printf("Opened %d files. Latest fd %d\n", i, fd);

        return 0;
    }

    printf("usage %s <num>\n", argv[0]);
}

