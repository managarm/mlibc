
#ifndef _TIME_H
#define _TIME_H

#include <bits/feature.h>
#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

// [7.27.1] Components of time

#define CLOCKS_PER_SEC 1000000000

#define TIME_UTC 1

// POSIX extensions.

#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID 3
#define CLOCK_MONOTONIC_RAW 4
#define CLOCK_REALTIME_COARSE 5
#define CLOCK_MONOTONIC_COARSE 6
#define CLOCK_BOOTTIME 7

#ifdef __cplusplus
extern "C" {
#endif

// [7.27.1] Components of time

// TODO: Use an integer type for clock_t?
typedef long int clock_t;

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

// [7.27.2] Time manipulation functions

clock_t clock(void);
double difftime(time_t a, time_t b);
time_t mktime(struct tm *ptr);
time_t time(time_t *timer);
int timespec_get(struct timespec *ptr, int base);

// [7.27.3] Time conversion functions

char *asctime(const struct tm *ptr);
char *ctime(const time_t *timer);
struct tm *gmtime(const time_t *timer);
struct tm *gmtime_r(const time_t *__restrict timer, struct tm *__restrict result);
struct tm *localtime(const time_t *timer);
size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict ptr);

void tzset(void);

#ifdef __cplusplus
}
#endif

// POSIX extensions.

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_time.h>
#endif // __MLIBC_POSIX_OPTION

#include <bits/ansi/clockid_t.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

extern int daylight;
extern long timezone;
extern char *tzname[2];

int nanosleep(const struct timespec *, struct timespec *);

int clock_getres(clockid_t, struct timespec *);
int clock_gettime(clockid_t, struct timespec *);
int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *);
int clock_settime(clockid_t, const struct timespec *);

struct tm *localtime_r(const time_t *, struct tm *);

char *strptime(const char *__restrict, const char *__restrict,
		struct tm *__restrict);

#ifdef __cplusplus
}
#endif

// GNU extensions.

#ifdef __cplusplus
extern "C" {
#endif

time_t timelocal(struct tm *);
time_t timegm(struct tm *);

#ifdef __cplusplus
}
#endif

// Linux extensions.

#ifdef __cplusplus
extern "C" {
#endif

struct itimerspec {
	struct timespec it_interval;
	struct timespec it_value;
};

#ifdef __cplusplus
}
#endif

#endif // _TIME_H
