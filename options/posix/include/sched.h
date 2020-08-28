
#ifndef _SCHED_H
#define _SCHED_H

#include <abi-bits/pid_t.h>
#include <bits/size_t.h>

// MISSING: time_t, struct timespec

// MISSING: POSIX [PS], [SS] and [TSP] options

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

#define CPU_SETSIZE 128
#define CPU_ISSET __mlibc_cpu_isset
#define CPU_COUNT __mlibc_cpu_count

#ifdef __cplusplus
extern "C" {
#endif

#define SCHED_OTHER 0
#define SCHED_FIFO 1
#define SCHED_RR 2
#define SCHED_BATCH 3
#define SCHED_IDLE 5
#define SCHED_DEADLINE 6
#define SCHED_RESET_ON_FORK 0x40000000

struct sched_param {
	int sched_priority;
};

int sched_yield();

struct __mlibc_cpu_set {
	unsigned long __bits[128/sizeof(long)];
};
typedef struct __mlibc_cpu_set cpu_set_t;

int sched_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);

int unshare(int flags);

int __mlibc_cpu_isset(int cpu, cpu_set_t *set);
int __mlibc_cpu_count(cpu_set_t *set);

#ifdef __cplusplus
}
#endif

#endif // _SCHED_H

