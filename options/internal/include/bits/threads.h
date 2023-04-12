#ifndef _INTERNAL_THREADS_H
#define _INTERNAL_THREADS_H

#include <bits/size_t.h>
#include <bits/cpu_set.h>
#include <bits/sigset_t.h>

// values for pthread_attr_{get,set}detachstate().
#define __MLIBC_THREAD_CREATE_JOINABLE 0
#define __MLIBC_THREAD_CREATE_DETACHED 1

struct sched_param {
	int sched_priority;
};

struct __mlibc_thread_data;

struct __mlibc_threadattr {
	size_t __mlibc_guardsize;
	size_t __mlibc_stacksize;
	void *__mlibc_stackaddr;
	int __mlibc_detachstate;
	int __mlibc_scope;
	int __mlibc_inheritsched;
	struct sched_param __mlibc_schedparam;
	int __mlibc_schedpolicy;
	cpu_set_t *__mlibc_cpuset;
	size_t __mlibc_cpusetsize;
	sigset_t __mlibc_sigmask;
	int __mlibc_sigmaskset;
};

#endif /* _INTERNAL_THREADS_H */
