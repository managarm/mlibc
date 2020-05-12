#ifndef _ABIBITS_RESOURCE_H
#define _ABIBITS_RESOURCE_H

#include <bits/posix/timeval.h>

struct rusage {
	struct timeval ru_utime;
	struct timeval ru_stime;
};

#endif // _ABIBITS_RESOURCE_H
