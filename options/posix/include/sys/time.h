#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <bits/ansi/time_t.h>
#include <bits/posix/suseconds_t.h>
#include <bits/posix/timeval.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO: this function is [OB]. disable it by default and add a macro to enable it
int gettimeofday(struct timeval *__restrict result, void *__restrict unused);

void timeradd(struct timeval *a, struct timeval *b, struct timeval *res);
void timersub(struct timeval *a, struct timeval *b, struct timeval *res);
void timerclear(struct timeval *tvp);
int timerisset(struct timeval *tvp);
//TODO: implement timercmp as a macro
//int timercmp(struct timeval *a, struct timeval *b, CMP);

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIME_H
