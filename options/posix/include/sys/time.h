#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <mlibc-config.h>

#include <sys/select.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/posix_sys_time.h>
#include <abi-bits/suseconds_t.h>

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
#include <abi-bits/time.h>
#endif /* defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024) */

#ifdef __cplusplus
extern "C" {
#endif

/* removed in POSIX 2024 */
#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};
#endif /* defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024) */

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
int gettimeofday(struct timeval *__restrict __result, void *__restrict __unused);

int getitimer(int __which, struct itimerval *__curr_value);
int setitimer(int __which, const struct itimerval *__new_value,
	struct itimerval *__old_value);
#endif /* defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024) */

#if defined(_DEFAULT_SOURCE)
int settimeofday(const struct timeval *__result, const struct timezone *__zone);

void timeradd(const struct timeval *__a, const struct timeval *__b, struct timeval *__res);
void timersub(const struct timeval *__a, const struct timeval *__b, struct timeval *__res);
void timerclear(struct timeval *__tvp);
int timerisset(struct timeval *__tvp);
#endif /* defined(_DEFAULT_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#if defined(_DEFAULT_SOURCE)
/* timercmp taken from musl */
#define timercmp(s,t,op) ((s)->tv_sec == (t)->tv_sec ? \
	(s)->tv_usec op (t)->tv_usec : (s)->tv_sec op (t)->tv_sec)
#endif /* defined(_DEFAULT_SOURCE) */

#if defined(_GNU_SOURCE)
/* The following 2 macros are taken from musl */
#define TIMEVAL_TO_TIMESPEC(tv, ts) ( \
	(ts)->tv_sec = (tv)->tv_sec, \
	(ts)->tv_nsec = (tv)->tv_usec * 1000, \
	(void)0 )
#define TIMESPEC_TO_TIMEVAL(tv, ts) ( \
	(tv)->tv_sec = (ts)->tv_sec, \
	(tv)->tv_usec = (ts)->tv_nsec / 1000, \
	(void)0 )
#endif /* defined(_GNU_SOURCE) */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TIME_H */
