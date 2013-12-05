#include <sys/ulimit/sys_ulimit.h>

#define RLIMIT_NPROC_MAX 1024
#define RLIMIT_NOFILE_MAX 1024
#define RLIMIT_STACK_MAX 8192

/* Resource hard limits */
const uint64_t RLIMIT_MAX[RLIMIT_NORES] = {
    [RLIMIT_NPROC] = RLIMIT_NPROC_MAX,
    [RLIMIT_NOFILE] = RLIMIT_NOFILE_MAX,
    [RLIMIT_STACK] = RLIMIT_STACK_MAX,
};

/* Resource soft limits */
uint64_t rlimit_cur[RLIMIT_NORES] = {
    [RLIMIT_NPROC] = RLIMIT_NPROC_MAX,
    [RLIMIT_NOFILE] = RLIMIT_NOFILE_MAX,
    [RLIMIT_STACK] = RLIMIT_STACK_MAX,
};

int sys_getrlimit(int resource, struct rlimit *rlim) {
    if(resource >= RLIMIT_NORES) {
        return -1;
    }

    if(rlim == NULL) {
        return -1;
    }

    rlim->rlim_cur = rlimit_cur[resource];
    rlim->rlim_max = RLIMIT_MAX[resource];

    return 0;
}

int sys_setrlimit(int resource, const struct rlimit *rlim) {
    if(resource >= RLIMIT_NORES) {
        return -1;
    }

    if(rlim == NULL) {
        return -1;
    }

    if(rlim->rlim_cur > RLIMIT_MAX[resource]) {
        return -1;
    }

    rlimit_cur[resource] = rlim->rlim_cur;

    return 0;
}

