#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <abi-bits/resource.h>
#include <bits/posix/id_t.h>
#include <bits/posix/suseconds_t.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/timeval.h>

#define PRIO_PROCESS 1
#define PRIO_PGRP 2
#define PRIO_USER 3

#define RLIM_INFINITY ((rlim_t)-1)
#define RLIM_SAVED_MAX ((rlim_t)-1)
#define RLIM_SAVED_CUR ((rlim_t)-1)

#define RUSAGE_SELF 1
#define RUSAGE_CHILDREN 2

#define RLIMIT_CORE 1
#define RLIMIT_CPU 2
#define RLIMIT_DATA 3
#define RLIMIT_FSIZE 4
#define RLIMIT_NOFILE 5
#define RLIMIT_STACK 6
#define RLIMIT_AS 7
#define RLIMIT_MEMLOCK 8
#define RLIMIT_RSS 9
#define RLIMIT_NPROC 10
#define RLIMIT_LOCKS 11
#define RLIMIT_SIGPENDING 12
#define RLIMIT_MSGQUEUE 13
#define RLIMIT_NICE 14
#define RLIMIT_RTPRIO 15
#define RLIMIT_NLIMITS 16

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long rlim_t;

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

int getpriority(int, id_t);
int setpriority(int, id_t, int);

int getrusage(int, struct rusage *);
int getrlimit(int, struct rlimit *);
int setrlimit(int, const struct rlimit *);

#ifdef __cplusplus
}
#endif

#endif // _SYS_RESOURCE_H
