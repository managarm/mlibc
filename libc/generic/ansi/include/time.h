
#ifndef _TIME_H
#define _TIME_H

#include <mlibc/null.h>
#include <mlibc/size_t.h>
#include <mlibc/time_t.h>
#include <mlibc/timespec.h>

// [7.27.1] Components of time

#define CLOCKS_PER_SEC 1000000000

#define TIME_UTC 1

// POSIX extensions.

#define CLOCK_REALTIME 1
#define CLOCK_MONOTONIC 2

#ifdef __cplusplus
extern "C" {
#endif

// [7.27.1] Components of time

// TODO: Use an integer type for clock_t?
typedef double clock_t;

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
struct tm *localtime(const time_t *timer);
size_t strftime(char *__restrict dest, size_t max_size,
		const char *__restrict format, const struct tm *__restrict ptr);

#ifdef __cplusplus
}
#endif

// POSIX extensions.

#include <mlibc/clockid_t.h>
#include <mlibc/suseconds_t.h>
#include <mlibc/timeval.h>

#define TIMER_ABSTIME 1

#ifdef __cplusplus
extern "C" {
#endif

int nanosleep(const struct timespec *, struct timespec *);

int clock_getres(clockid_t, struct timespec *);
int clock_gettime(clockid_t, struct timespec *);
int clock_nanosleep(clockid_t, int, const struct timespec *, struct timespec *);
int clock_settime(clockid_t, const struct timespec *);

int utimes(const char *, const struct timeval[2]);

#ifdef __cplusplus
}
#endif

#endif // _TIME_H
