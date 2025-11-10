
#ifndef _MLIBC_ANSI_TIMESPEC_H
#define _MLIBC_ANSI_TIMESPEC_H

#include <bits/ansi/time_t.h>

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

#endif /* _MLIBC_ANSI_TIMESPEC_H */

