#ifndef _TIME_H
#define _TIME_H

#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <mlibc-config.h>

/* [7.27.1] Components of time */

#define CLOCKS_PER_SEC ((clock_t)1000000)

#define TIME_UTC 1

/* POSIX extensions. */

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7
#define CLOCK_REALTIME_ALARM 8
#define CLOCK_BOOTTIME_ALARM 9
#define CLOCK_TAI 11

#ifdef __cplusplus
extern "C" {
#endif

/* [7.27.1] Components of time */

typedef long clock_t; /* Matches Linux' ABI. */

struct tm {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
	long int tm_gmtoff;
	const char *tm_zone;
};

#ifndef __MLIBC_ABI_ONLY

/* [7.27.2] Time manipulation functions */

clock_t clock(void);
double difftime(time_t __a, time_t __b);
time_t mktime(struct tm *__ptr);
time_t time(time_t *__timer);
int timespec_get(struct timespec *__ptr, int __base);

/* [7.27.3] Time conversion functions */

char *asctime(const struct tm *__ptr);
char *ctime(const time_t *__timer);
struct tm *gmtime(const time_t *__timer);
struct tm *gmtime_r(const time_t *__restrict __timer, struct tm *__restrict __result);
struct tm *localtime(const time_t *__timer);
size_t strftime(char *__restrict __dest, size_t __max_size,
		const char *__restrict __format, const struct tm *__restrict __ptr);

void tzset(void);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

/* POSIX extensions. */

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_time.h>
#	include <bits/posix/timer_t.h>
#endif /* __MLIBC_POSIX_OPTION */

#include <abi-bits/clockid_t.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

extern int daylight;
extern long timezone;
extern char *tzname[2];

int nanosleep(const struct timespec *__req, struct timespec *__rem);

int clock_getres(clockid_t __clockid, struct timespec *__res);
int clock_gettime(clockid_t __clockid, struct timespec *__res);
int clock_nanosleep(clockid_t __clockid, int __flags, const struct timespec *__req, struct timespec *__rem);
int clock_settime(clockid_t __clockid, const struct timespec *__time);

struct tm *localtime_r(const time_t *__timer, struct tm *__buf);
char *asctime_r(const struct tm *__tm, char *__buf);
char *ctime_r(const time_t *__timer, char *__buf);

#if __MLIBC_POSIX_OPTION
#include <abi-bits/pid_t.h>
char *strptime(const char *__restrict __buf, const char *__restrict __format,
		struct tm *__restrict __tm);
int clock_getcpuclockid(pid_t __pid, clockid_t *__clockid);
#endif /* __MLIBC_POSIX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

/* GNU extensions. */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

time_t timelocal(struct tm *__tm);
time_t timegm(struct tm *__tm);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H */
