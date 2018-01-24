
#ifndef _SCHED_H
#define _SCHED_H

#include <bits/posix/pid_t.h>

// MISSING: time_t, struct timespec

// MISSING: POSIX [PS], [SS] and [TSP] options

#ifdef __cplusplus
extern "C" {
#endif

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_IDLE 5
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

int sched_yield();

#ifdef __cplusplus
}
#endif

#endif // _SCHED_H

