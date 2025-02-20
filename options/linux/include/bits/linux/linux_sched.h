
#ifndef _BITS_LINUX_SCHED_H
#define _BITS_LINUX_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <abi-bits/pid_t.h>
#include <bits/size_t.h>
#include <bits/linux/cpu_set.h>

#define CLONE_VM 0x00000100
#define CLONE_FS 0x00000200
#define CLONE_FILES	0x00000400
#define CLONE_SIGHAND 0x00000800
#define CLONE_PTRACE 0x00002000
#define CLONE_VFORK 0x00004000
#define CLONE_PARENT 0x00008000
#define CLONE_THREAD 0x00010000
#define CLONE_NEWNS 0x00020000
#define CLONE_SYSVSEM 0x00040000
#define CLONE_SETTLS 0x00080000
#define CLONE_PARENT_SETTID 0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED 0x00400000
#define CLONE_UNTRACED 0x00800000
#define CLONE_CHILD_SETTID 0x01000000
#define CLONE_NEWCGROUP 0x02000000
#define CLONE_NEWUTS 0x04000000
#define CLONE_NEWIPC 0x08000000
#define CLONE_NEWUSER 0x10000000
#define CLONE_NEWPID 0x20000000
#define CLONE_NEWNET 0x40000000
#define CLONE_IO 0x80000000

#ifndef __MLIBC_ABI_ONLY

int sched_getscheduler(pid_t __pid);
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
