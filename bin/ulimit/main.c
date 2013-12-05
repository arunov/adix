#include <syscall.h>
#include <string.h>
#include <ulimit.h>
#include <stdio.h>

char *res_name[] = {"max processes", "open files", "stack size"};
char *res_unit[] = {"", "", "kbytes"};
char *res_flag[] = {"-u", "-n", "-s"};

void printulimit(int res) {

    struct rlimit x;
    getrlimit(res, &x);
    printf("%s (", res_name[res]);
    if(res_unit[res][0] != '\0') {
        printf("%s, ", res_unit[res]);
    }
    printf("%s) %d\n", res_flag[res], x.rlim_cur);
}

void printallulimits() {

    for(int i = 0; i < RLIMIT_NORES; i ++) {
        printulimit(i);
    }

}

int main(int argc, char *argv[]) {

    if(argc == 1 || (argc == 2 && str_equal(argv[1], "-a"))) {

        printallulimits();
        return 0;
    }

    struct rlimit x;

    if(argc == 2 || argc == 3) {
        int resid;
        for(resid = 0 ; resid < RLIMIT_NORES; resid ++) {
            if(str_equal(argv[1], res_flag[resid]))
                break;
        }

        if(resid == RLIMIT_NORES) {
            goto error;
        }

        if(argc == 3) {

            int newlim = atoi(argv[2]);

            x.rlim_cur = (uint64_t) newlim;
            if(-1 == setrlimit(resid, &x)) {
                printf("setrlimit failed\n");
            }
            return 0;
        }

        // argc = 2
        printulimit(resid);
        return 0;
    }

error:
    printf("usage:\n%s [-a|[%s|%s|%s [<new-limit>]]]\n", argv[0], res_flag[0],
                                                    res_flag[1], res_flag[2]);

    return 1;
}

