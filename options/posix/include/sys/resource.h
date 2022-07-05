#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <limits.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/resource.h>
#include <bits/posix/id_t.h>
#include <abi-bits/suseconds_t.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/timeval.h>

#define PRIO_PROCESS 1
#define PRIO_PGRP 2
#define PRIO_USER 3

#define RLIM_INFINITY ((rlim_t)-1)
#define RLIM_SAVED_MAX ((rlim_t)-1)
#define RLIM_SAVED_CUR ((rlim_t)-1)

#define RLIM_NLIMITS RLIMIT_NLIMITS

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long rlim_t;

_Static_assert(sizeof(long) * CHAR_BIT == 64, "long * CHAR_BIT is not 64, we should fix this on this architecture!");

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

int getpriority(int, id_t);
int setpriority(int, id_t, int);

int getrusage(int, struct rusage *);
int getrlimit(int, struct rlimit *);
int setrlimit(int, const struct rlimit *);

int prlimit(pid_t pid, int resource, const struct rlimit *new_limits, struct rlimit *old_limits);

#define rlimit64 rlimit

#ifdef __cplusplus
}
#endif

#endif // _SYS_RESOURCE_H
