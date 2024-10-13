#ifndef _ABIBITS_TIME_H
#define _ABIBITS_TIME_H

#include <bits/posix/timeval.h>

struct itimerval {
	struct timeval it_interval;	/* Interval for periodic timer */
	struct timeval it_value;	/* Time until next expiration */
};

#define ITIMER_REAL	0
#define ITIMER_VIRTUAL	1
#define ITIMER_PROF	2

#endif /* _ABIBITS_TIME_H */
