#ifndef MLIBC_TIMEVAL_H
#define MLIBC_TIMEVAL_H

struct timeval {
	time_t tv_sec;
	suseconds_t tv_usec;
};

#endif // MLIBC_TIMEVAL_H
