#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <abi-bits/pid_t.h>
#include <abi-bits/resource.h>
#include <bits/posix/id_t.h>
#include <abi-bits/suseconds_t.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/timeval.h>

#define PRIO_PROCESS 1
#define PRIO_PGRP 2
#define PRIO_USER 3

#define PRIO_MIN (-20)
#define PRIO_MAX 20

#define RLIM_INFINITY ((rlim_t)-1)
#define RLIM_SAVED_MAX ((rlim_t)-1)
#define RLIM_SAVED_CUR ((rlim_t)-1)

#define RLIM_NLIMITS RLIMIT_NLIMITS

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long rlim_t;

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

#define rlimit64 rlimit

#ifndef __MLIBC_ABI_ONLY

int getpriority(int, id_t);
int setpriority(int, id_t, int);

int getrusage(int, struct rusage *);
int getrlimit(int, struct rlimit *);
int getrlimit64(int, struct rlimit *);
int setrlimit(int, const struct rlimit *);
int setrlimit64(int, const struct rlimit *);

int prlimit(pid_t pid, int resource, const struct rlimit *new_limits, struct rlimit *old_limits);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_RESOURCE_H
