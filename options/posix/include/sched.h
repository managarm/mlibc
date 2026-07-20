
#ifndef _SCHED_H
#define _SCHED_H

#include <mlibc-config.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/sched_param.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/threads.h>
#include <bits/size_t.h>

/* MISSING: parts of POSIX [PS], [SS] and [TSP] options */

#ifdef __cplusplus
extern "C" {
#endif

#if __MLIBC_LINUX_OPTION && defined(_GNU_SOURCE)
#include <bits/linux/linux_sched.h>
#include <bits/linux/cpu_set.h>
#endif

#ifndef __MLIBC_ABI_ONLY

int sched_yield(void);

int sched_get_priority_max(int __policy);
int sched_get_priority_min(int __policy);

int sched_setscheduler(pid_t __pid, int __policy, const struct sched_param *__param);
int sched_getscheduler(pid_t __pid);

int sched_getparam(pid_t __pid, struct sched_param *__param);
int sched_setparam(pid_t __pid, const struct sched_param *__param);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SCHED_H */

