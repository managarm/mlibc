#ifndef _ABIBITS_SCHED_PARAM_H
#define _ABIBITS_SCHED_PARAM_H

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2

#ifdef __cplusplus
extern "C" {
#endif

struct sched_param {
	int sched_priority;
	int sched_priority_min;
	int sched_priority_max;
};

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SCHED_PARAM_H */
