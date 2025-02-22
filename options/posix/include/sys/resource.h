#ifndef _SYS_RESOURCE_H
#define _SYS_RESOURCE_H

#include <abi-bits/pid_t.h>
#include <abi-bits/resource.h>
#include <abi-bits/rlim_t.h>
#include <abi-bits/suseconds_t.h>
#include <bits/ansi/time_t.h>
#include <bits/posix/id_t.h>
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

struct rlimit {
	rlim_t rlim_cur;
	rlim_t rlim_max;
};

#ifndef __MLIBC_ABI_ONLY

int getpriority(int __which, id_t __who);
int setpriority(int __which, id_t __who, int __prio);

int getrusage(int __who, struct rusage *__usage);
int getrlimit(int __resource, struct rlimit *__rlim);
int getrlimit64(int __resource, struct rlimit *__rlim);
int setrlimit(int __resource, const struct rlimit *__rlim);
int setrlimit64(int __resource, const struct rlimit *__rlim);

int prlimit(pid_t __pid, int __resource, const struct rlimit *__new_limits, struct rlimit *__old_limits);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_RESOURCE_H */
