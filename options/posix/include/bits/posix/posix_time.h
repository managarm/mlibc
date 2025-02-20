#ifndef MLIBC_POSIX_TIME_H
#define MLIBC_POSIX_TIME_H

#include <abi-bits/clockid_t.h>
#include <abi-bits/sigevent.h>
#include <bits/ansi/timespec.h>
#include <bits/posix/timer_t.h>
#include <bits/posix/timeval.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
};

#ifndef __MLIBC_ABI_ONLY

int timer_getoverrun(timer_t __timerid);

int utimes(const char *__filename, const struct timeval __tv[2]);

/* Not standardized, Linux and BSDs have it */
int futimes(int __fd, const struct timeval __tv[2]);
int lutimes(const char *__filename, const struct timeval __tv[2]);

int timer_create(clockid_t __clockid, struct sigevent *__restrict __sevp, timer_t *__restrict __timerid);
int timer_settime(timer_t __timerid, int __flags, const struct itimerspec *__restrict __new_value,
	struct itimerspec *__restrict __old_value);
int timer_gettime(timer_t __timerid, struct itimerspec *__curr_value);
int timer_delete(timer_t __timerid);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_TIME_H */
