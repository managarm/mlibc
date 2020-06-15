#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <abi-bits/time.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/suseconds_t.h>
#include <bits/posix/timeval.h>

#ifdef __cplusplus
extern "C" {
#endif

struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};

// TODO: this function is [OB]. disable it by default and add a macro to enable it
int gettimeofday(struct timeval *__restrict result, void *__restrict unused);

void timeradd(struct timeval *a, struct timeval *b, struct timeval *res);
void timersub(struct timeval *a, struct timeval *b, struct timeval *res);
void timerclear(struct timeval *tvp);
int timerisset(struct timeval *tvp);
//TODO: implement timercmp as a macro
//int timercmp(struct timeval *a, struct timeval *b, CMP);

int getitimer(int which, struct itimerval *curr_value);
int setitimer(int which, const struct itimerval *new_value,
	struct itimerval *old_value);

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIME_H
