#ifndef _INTERNAL_THREADS_H
#define _INTERNAL_THREADS_H

#include <abi-bits/clockid_t.h>
#include <bits/size_t.h>
#include <bits/cpu_set.h>
#include <bits/sigset_t.h>

/* values for pthread_attr_{get,set}detachstate(). */
#define __MLIBC_THREAD_CREATE_JOINABLE 0
#define __MLIBC_THREAD_CREATE_DETACHED 1

/* values for pthread_mutexattr_{get,set}type(). */
#define __MLIBC_THREAD_MUTEX_DEFAULT 0
#define __MLIBC_THREAD_MUTEX_NORMAL 0
#define __MLIBC_THREAD_MUTEX_ERRORCHECK 1
#define __MLIBC_THREAD_MUTEX_RECURSIVE 2

/* values for pthread_mutexattr_{get,set}pshared(). */
#define __MLIBC_THREAD_PROCESS_PRIVATE 0
#define __MLIBC_THREAD_PROCESS_SHARED 1

/* values for pthread_mutexattr_{get,set}robust(). */
#define __MLIBC_THREAD_MUTEX_STALLED 0
#define __MLIBC_THREAD_MUTEX_ROBUST 1

/* Values for pthread_mutexattr_{get,set}protocol() */
#define __MLIBC_THREAD_PRIO_NONE 0
#define __MLIBC_THREAD_PRIO_INHERIT 1
#define __MLIBC_THREAD_PRIO_PROTECT 2

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

struct __mlibc_mutex {
	unsigned int __mlibc_state;
	unsigned int __mlibc_recursion;
	unsigned int __mlibc_flags;
	int __mlibc_prioceiling;
};

struct __mlibc_mutexattr {
	int __mlibc_type;
	int __mlibc_robust;
	int __mlibc_protocol;
	int __mlibc_pshared;
	int __mlibc_prioceiling;
};

struct __mlibc_cond {
	unsigned int __mlibc_seq;
	unsigned int __mlibc_flags;
	clockid_t __mlibc_clock;
};

struct __mlibc_condattr {
	int __mlibc_pshared;
	clockid_t __mlibc_clock;
};

#endif /* _INTERNAL_THREADS_H */
