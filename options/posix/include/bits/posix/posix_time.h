#ifndef MLIBC_POSIX_TIME_H
#define MLIBC_POSIX_TIME_H

#include <bits/posix/timer_t.h>
#include <bits/posix/timeval.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

int timer_getoverrun(timer_t __timerid);

int utimes(const char *__filename, const struct timeval __tv[2]);

/* Not standardized, Linux and BSDs have it */
int futimes(int __fd, const struct timeval __tv[2]);
int lutimes(const char *__filename, const struct timeval __tv[2]);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_TIME_H */
