
#ifndef _MLIBC_ANSI_TIMESPEC_H
#define _MLIBC_ANSI_TIMESPEC_H

#include <bits/ansi/time_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __INTPTR_WIDTH__ == 32
struct timespec {
	time_t tv_sec;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	long tv_nsec;
	int :32;
#else
	int :32;
	long tv_nsec;
#endif
};
#else
struct timespec {
	time_t tv_sec;
	long tv_nsec;
};
#endif

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_ANSI_TIMESPEC_H */

