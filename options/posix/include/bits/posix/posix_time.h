#ifndef _MLIBC_POSIX_TIME_H
#define _MLIBC_POSIX_TIME_H

#include <abi-bits/clockid_t.h>
#include <abi-bits/sigevent.h>
#include <bits/size_t.h>
#include <bits/ansi/timespec.h>
#include <bits/ansi/tm.h>
#include <bits/posix/timer_t.h>
#include <bits/posix/locale_t.h>

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

int timer_create(clockid_t __clockid, struct sigevent *__restrict __sevp, timer_t *__restrict __timerid);
int timer_settime(timer_t __timerid, int __flags, const struct itimerspec *__restrict __new_value,
	struct itimerspec *__restrict __old_value);
int timer_gettime(timer_t __timerid, struct itimerspec *__curr_value);
int timer_delete(timer_t __timerid);

size_t strftime_l(char *__restrict __s, size_t __maxsize, const char *__restrict __format, const struct tm *__restrict __timeptr, locale_t __locale);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_POSIX_TIME_H */
