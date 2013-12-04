#ifndef _SYS_ULIMIT_H
#define _SYS_ULIMIT_H

#include <defs.h>
#include <ulimit.h>

/* Resource soft limits */
uint64_t rlimit_cur[RLIMIT_NORES];

/* Resource hard limits */
const uint64_t RLIMIT_MAX[RLIMIT_NORES];

/**
 * Get resource limit
 * @param resource resource
 * @param rlim     output
 * @return         OK or ERROR
 */
int sys_getrlimit(int resource, struct rlimit *rlim);

/**
 * Set resource limit
 * @param resource resource
 * @param rlim     input
 * @return         OK or ERROR
 */
int sys_setrlimit(int resource, const struct rlimit *rlim);

#endif

