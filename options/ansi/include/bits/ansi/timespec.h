
#ifndef MLIBC_TIMESPEC_H
#define MLIBC_TIMESPEC_H

#include <bits/ansi/time_t.h>

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

#endif /* MLIBC_TIMESPEC_H */

