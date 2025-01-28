#ifndef MLIBC_TIMEVAL_H
#define MLIBC_TIMEVAL_H

#include <bits/ansi/time_t.h>
#include <abi-bits/suseconds_t.h>

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

#endif /* MLIBC_TIMEVAL_H */
