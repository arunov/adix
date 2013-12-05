#ifndef _ULIMIT_H
#define _ULIMIT_H

#include<defs.h>

/* Resource ids */
enum {

    /* Number of processes per user */
    RLIMIT_NPROC,

    /* Number of files per process */
    RLIMIT_NOFILE,

    /* Stack size in kbytes per process */
    RLIMIT_STACK,

    /* Number of resources in the list */
    RLIMIT_NORES,
};

/**
 * Struct to obtain soft and hard limits of resource
 */
struct rlimit {
    uint64_t rlim_cur;
    uint64_t rlim_max;
};

/**
 * Get resource limit
 * @param resource resource
 * @param rlim     output
 * @return         OK or ERROR
 */
int getrlimit(int resource, struct rlimit *rlim);

/**
 * Set resource limit
 * @param resource resource
 * @param rlim     input
 * @return         OK or ERROR
 */
int setrlimit(int resource, const struct rlimit *rlim);

#endif

