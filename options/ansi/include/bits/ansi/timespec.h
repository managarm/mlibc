
#ifndef _MLIBC_ANSI_TIMESPEC_H
#define _MLIBC_ANSI_TIMESPEC_H

#include <bits/ansi/time_t.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timespec {
	time_t tv_sec;
	long tv_nsec;
};

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_ANSI_TIMESPEC_H */

