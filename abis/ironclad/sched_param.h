#ifndef _ABIBITS_SCHED_PARAM_H
#define _ABIBITS_SCHED_PARAM_H

#include <bits/types.h>

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_IDLE 5
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

#ifdef __cplusplus
extern "C" {
#endif

struct sched_param {
	int sched_priority;

	/* [SS|TSP] members missing */

	__mlibc_uint32 __reserved[15];
} __attribute__((aligned(__INTPTR_WIDTH__ / 8)));

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SCHED_PARAM_H */
