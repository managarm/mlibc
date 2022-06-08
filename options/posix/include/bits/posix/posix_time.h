#ifndef MLIBC_POSIX_TIME_H
#define MLIBC_POSIX_TIME_H

#include <bits/posix/timeval.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

int utimes(const char *, const struct timeval[2]);

// Not standardized, Linux and BSDs have it
int futimes(int, const struct timeval[2]);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_TIME_H
