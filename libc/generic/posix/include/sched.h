
#ifndef _SCHED_H
#define _SCHED_H

#include <mlibc/pid_t.h>

// MISSING: time_t, struct timespec

// MISSING: POSIX [PS], [SS] and [TSP] options

#ifdef __cplusplus
extern "C" {
#endif

int sched_yield();

#ifdef __cplusplus
}
#endif

#endif // _SCHED_H

