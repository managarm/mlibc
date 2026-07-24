
#ifndef _BITS_LINUX_SCHED_H
#define _BITS_LINUX_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/clone-flags.h>
#include <abi-bits/pid_t.h>
#include <bits/size_t.h>
#include <bits/linux/cpu_set.h>

#ifndef __MLIBC_ABI_ONLY

int sched_setaffinity(pid_t __pid, size_t __cpusetsize, const cpu_set_t *__mask);
int sched_getaffinity(pid_t __pid, size_t __cpusetsize, cpu_set_t *__mask);

int unshare(int flags);
int clone(int (*)(void *), void *, int, void *, ...);

/* Glibc extension */
int sched_getcpu(void);

#if defined(_GNU_SOURCE)
int setns(int fd, int nstype);
#endif /* _GNU_SOURCE */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_SCHED_H */
