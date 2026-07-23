#ifndef _MLIBC_INTERNAL_TIMEVAL_H
#define _MLIBC_INTERNAL_TIMEVAL_H

#include <bits/ansi/time_t.h>
#include <abi-bits/suseconds_t.h>

struct timeval {
	time_t tv_sec;
#if __INTPTR_WIDTH__ == 32
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	suseconds_t tv_usec;
	int :32;
#else
	int :32;
	suseconds_t tv_usec;
#endif
#else
	suseconds_t tv_usec;
#endif
};

#endif /* _MLIBC_INTERNAL_TIMEVAL_H */
