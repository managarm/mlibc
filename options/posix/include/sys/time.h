#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <abi-bits/time.h>
#include <abi-bits/signal.h>
#include <abi-bits/clockid_t.h>
#include <bits/ansi/time_t.h>
#include <abi-bits/suseconds_t.h>
#include <bits/posix/timer_t.h>
#include <bits/posix/timeval.h>

#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

#ifndef __MLIBC_ABI_ONLY

/* TODO: this function is [OB]. disable it by default and add a macro to enable it */
int gettimeofday(struct timeval *__restrict __result, void *__restrict __unused);
int settimeofday(const struct timeval *__result, const struct timezone *__zone);

void timeradd(const struct timeval *__a, const struct timeval *__b, struct timeval *__res);
void timersub(const struct timeval *__a, const struct timeval *__b, struct timeval *__res);
void timerclear(struct timeval *__tvp);
int timerisset(struct timeval *__tvp);

#endif /* !__MLIBC_ABI_ONLY */

/* timercmp taken from musl */
#define timercmp(s,t,op) ((s)->tv_sec == (t)->tv_sec ? \
	(s)->tv_usec op (t)->tv_usec : (s)->tv_sec op (t)->tv_sec)

#ifndef __MLIBC_ABI_ONLY

int getitimer(int __which, struct itimerval *__curr_value);
int setitimer(int __which, const struct itimerval *__new_value,
	struct itimerval *__old_value);

#endif /* !__MLIBC_ABI_ONLY */

/* The following 2 macros are taken from musl */
#define TIMEVAL_TO_TIMESPEC(tv, ts) ( \
	(ts)->tv_sec = (tv)->tv_sec, \
	(ts)->tv_nsec = (tv)->tv_usec * 1000, \
	(void)0 )
#define TIMESPEC_TO_TIMEVAL(tv, ts) ( \
	(tv)->tv_sec = (ts)->tv_sec, \
	(tv)->tv_usec = (ts)->tv_nsec / 1000, \
	(void)0 )

#ifdef __cplusplus
}
#endif

#endif /* _SYS_TIME_H */
