#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <abi-bits/time.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/suseconds_t.h>
#include <bits/posix/timeval.h>

#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

// TODO: this function is [OB]. disable it by default and add a macro to enable it
int gettimeofday(struct timeval *__restrict result, void *__restrict unused);

void timeradd(const struct timeval *a, const struct timeval *b, struct timeval *res);
void timersub(const struct timeval *a, const struct timeval *b, struct timeval *res);
void timerclear(struct timeval *tvp);
int timerisset(struct timeval *tvp);

// timercmp taken from musl
#define timercmp(s,t,op) ((s)->tv_sec == (t)->tv_sec ? \
	(s)->tv_usec op (t)->tv_usec : (s)->tv_sec op (t)->tv_sec)

int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value,
	struct itimerval *old_value);

// The following 2 macros are taken from musl
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

#endif // _SYS_TIME_H
