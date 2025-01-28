
#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <abi-bits/clockid_t.h>
#include <bits/cpu_set.h>
/* TODO: pthread is not required to define size_t. */
#include <bits/size_t.h>
#include <bits/posix/pthread_t.h>
#include <bits/threads.h>
#include <mlibc-config.h>

#include <signal.h>
#include <stdint.h>

/* pthread.h is required to include sched.h and time.h */
#include <sched.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PTHREAD_CREATE_JOINABLE __MLIBC_THREAD_CREATE_JOINABLE
#define PTHREAD_CREATE_DETACHED __MLIBC_THREAD_CREATE_DETACHED

/* Values for pthread_attr_{get,set}scope */
#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_SCOPE_PROCESS 1

/* Values for pthread_attr_{get,set}inheritsched */
#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

/* values for pthread_{get,set}canceltype(). */
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCHRONOUS 1

/* values for pthread_{get,set}cancelstate(). */
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

/* values for pthread_mutexattr_{get,set}type(). */
#define PTHREAD_MUTEX_DEFAULT __MLIBC_THREAD_MUTEX_DEFAULT
#define PTHREAD_MUTEX_NORMAL __MLIBC_THREAD_MUTEX_NORMAL
#define PTHREAD_MUTEX_ERRORCHECK __MLIBC_THREAD_MUTEX_ERRORCHECK
#define PTHREAD_MUTEX_RECURSIVE __MLIBC_THREAD_MUTEX_RECURSIVE

/* values for pthread_mutexattr_{get,set}robust(). */
#define PTHREAD_MUTEX_STALLED __MLIBC_THREAD_MUTEX_STALLED
#define PTHREAD_MUTEX_ROBUST __MLIBC_THREAD_MUTEX_ROBUST

/* values for pthread_mutexattr_{get,set}pshared(). */
#define PTHREAD_PROCESS_PRIVATE __MLIBC_THREAD_PROCESS_PRIVATE
#define PTHREAD_PROCESS_SHARED __MLIBC_THREAD_PROCESS_SHARED

/* Values for pthread_mutexattr_{get,set}protocol() */
#define PTHREAD_PRIO_NONE __MLIBC_THREAD_PRIO_NONE
#define PTHREAD_PRIO_INHERIT __MLIBC_THREAD_PRIO_INHERIT
#define PTHREAD_PRIO_PROTECT __MLIBC_THREAD_PRIO_PROTECT

#define PTHREAD_ONCE_INIT {0}
#define PTHREAD_COND_INITIALIZER {0}
#define PTHREAD_MUTEX_INITIALIZER {0, 0, 0, 0}
#define PTHREAD_RWLOCK_INITIALIZER {0, 0, 0}

#define PTHREAD_CANCELED ((void*) -1)

#define PTHREAD_BARRIER_SERIAL_THREAD -1

/* values for pthread_key */
#define PTHREAD_DESTRUCTOR_ITERATIONS 8

#define PTHREAD_INHERIT_SCHED 0
#define PTHREAD_EXPLICIT_SCHED 1

#define PTHREAD_STACK_MIN 16384

#define PTHREAD_ATTR_NO_SIGMASK_NP (-1)

/* TODO: move to own file and include in sys/types.h */
typedef struct __mlibc_threadattr pthread_attr_t;

typedef uintptr_t pthread_key_t;

struct __mlibc_once {
	unsigned int __mlibc_done;
};
typedef struct __mlibc_once pthread_once_t;

typedef struct __mlibc_mutexattr pthread_mutexattr_t;

typedef struct __mlibc_mutex pthread_mutex_t;

typedef struct __mlibc_condattr pthread_condattr_t;

typedef struct __mlibc_cond pthread_cond_t;

struct  __mlibc_barrierattr_struct {
	int __mlibc_pshared;
};
typedef struct __mlibc_barrierattr_struct pthread_barrierattr_t;

struct __mlibc_barrier {
	unsigned int __mlibc_waiting;
	unsigned int __mlibc_inside;
	unsigned int __mlibc_count;
	unsigned int __mlibc_seq;
	unsigned int __mlibc_flags;
};
typedef struct __mlibc_barrier pthread_barrier_t;

struct __mlibc_fair_rwlock {
	unsigned int __mlibc_m; /* Mutex. */
	unsigned int __mlibc_rc; /* Reader count (not reference count). */
	unsigned int __mlibc_flags;
};
typedef struct __mlibc_fair_rwlock pthread_rwlock_t;

struct __mlibc_rwlockattr {
	int __mlibc_pshared;
};
typedef struct __mlibc_rwlockattr pthread_rwlockattr_t;

#ifndef __MLIBC_ABI_ONLY

/* ---------------------------------------------------------------------------- */
/* pthread_attr and pthread functions. */
/* ---------------------------------------------------------------------------- */

/* pthread_attr functions. */
int pthread_attr_init(pthread_attr_t *__attr);
int pthread_attr_destroy(pthread_attr_t *__attr);

int pthread_attr_getdetachstate(const pthread_attr_t *__attr, int *__state);
int pthread_attr_setdetachstate(pthread_attr_t *__attr, int __state);

int pthread_attr_getstacksize(const pthread_attr_t *__restrict __attr, size_t *__restrict __stacksize);
int pthread_attr_setstacksize(pthread_attr_t *__attr, size_t __stacksize);

int pthread_attr_getstackaddr(const pthread_attr_t *__attr, void **__stackaddr);
int pthread_attr_setstackaddr(pthread_attr_t *__attr, void *__stackaddr);

int pthread_attr_getstack(const pthread_attr_t *__attr, void **__stackaddr, size_t *__stacksize);
int pthread_attr_setstack(pthread_attr_t *__attr, void *__stackaddr, size_t __stacksize);

int pthread_attr_getguardsize(const pthread_attr_t *__restrict __attr, size_t *__restrict __guardsize);
int pthread_attr_setguardsize(pthread_attr_t *__attr, size_t __guardsize);

int pthread_attr_getscope(const pthread_attr_t *__attr, int *__scope);
int pthread_attr_setscope(pthread_attr_t *__attr, int __scope);

int pthread_attr_getschedparam(const pthread_attr_t *__restrict __attr, struct sched_param *__restrict __param);
int pthread_attr_setschedparam(pthread_attr_t *__restrict __attr, const struct sched_param *__restrict __param);

int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict __attr, int *__restrict __schedpolicy);
int pthread_attr_setschedpolicy(pthread_attr_t *__restrict __attr, int __schedpolicy);

int pthread_attr_getinheritsched(const pthread_attr_t *__restrict __attr, int *__restrict __inheritsched);
int pthread_attr_setinheritsched(pthread_attr_t *__restrict __attr, int __inheritsched);

#if __MLIBC_LINUX_OPTION
int pthread_attr_getaffinity_np(const pthread_attr_t *__restrict __attr, size_t __cpusetsize, cpu_set_t *__restrict __cpuset);
int pthread_attr_setaffinity_np(pthread_attr_t *__restrict __attr, size_t __cpusetsize, const cpu_set_t *__restrict __cpuset);

int pthread_attr_getsigmask_np(const pthread_attr_t *__restrict __attr, sigset_t *__restrict __sigmask);
int pthread_attr_setsigmask_np(pthread_attr_t *__restrict __attr, const sigset_t *__restrict __sigmask);

int pthread_getattr_np(pthread_t __thrd, pthread_attr_t *__attr);

int pthread_getaffinity_np(pthread_t __thrd, size_t __cpusetsize, cpu_set_t *__cpuset);
int pthread_setaffinity_np(pthread_t __thrd, size_t __cpusetsize, const cpu_set_t *__cpuset);
#endif /* __MLIBC_LINUX_OPTION */

/* pthread functions. */
int pthread_create(pthread_t *__restrict __thrd, const pthread_attr_t *__restrict __attr,
		void *(*__fn) (void *__arg), void *__restrict __arg);
pthread_t pthread_self(void);
int pthread_equal(pthread_t __a, pthread_t __b);
__attribute__ ((__noreturn__)) void pthread_exit(void *__arg);

int pthread_join(pthread_t __thrd, void **__res);
int pthread_detach(pthread_t __thrd);

void pthread_cleanup_push(void (*__fn) (void *__arg), void *__arg);
void pthread_cleanup_pop(int __execute);

int pthread_setname_np(pthread_t __thrd, const char *__name);
int pthread_getname_np(pthread_t __thrd, char *__name, size_t __size);

int pthread_attr_setstack(pthread_attr_t *__attr, void *__stackaddr, size_t __stacksize);
int pthread_attr_getstack(const pthread_attr_t *, void **__stackaddr, size_t *__stacksize);

int pthread_getattr_np(pthread_t __thrd, pthread_attr_t *__attr);

int pthread_setschedparam(pthread_t __thrd, int __policy, const struct sched_param *__param);
int pthread_getschedparam(pthread_t __thrd, int *__policy, struct sched_param *__param);

int pthread_setcanceltype(int __type, int *__oldtype);
int pthread_setcancelstate(int __state, int *__oldstate);
void pthread_testcancel(void);
int pthread_cancel(pthread_t __thrd);

int pthread_atfork(void (*__prepare) (void), void (*__parent) (void), void (*__child) (void));

/* ---------------------------------------------------------------------------- */
/* pthread_key functions. */
/* ---------------------------------------------------------------------------- */

int pthread_key_create(pthread_key_t *__key, void (*__destructor) (void *__data));
int pthread_key_delete(pthread_key_t __key);

void *pthread_getspecific(pthread_key_t __key);
int pthread_setspecific(pthread_key_t __key, const void *__data);

/* ---------------------------------------------------------------------------- */
/* pthread_once functions. */
/* ---------------------------------------------------------------------------- */

int pthread_once(pthread_once_t *__once, void (*__fn) (void));

/* ---------------------------------------------------------------------------- */
/* pthread_mutexattr and pthread_mutex functions. */
/* ---------------------------------------------------------------------------- */

/* pthread_mutexattr functions */
int pthread_mutexattr_init(pthread_mutexattr_t *__attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *__attr);

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict __attr, int *__restrict __type);
int pthread_mutexattr_settype(pthread_mutexattr_t *__attr, int __type);

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict __attr, int *__restrict __robust);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *__attr, int __robust);

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *__attr, int *__pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *__attr, int __pshared);

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict __attr, int *__restrict __protocol);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *__attr, int __protocol);

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *__attr, int *__prioceiling);
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *__attr, int __prioceiling);

/* pthread_mutex functions */
int pthread_mutex_init(pthread_mutex_t *__restrict __mtx, const pthread_mutexattr_t *__restrict __attr);
int pthread_mutex_destroy(pthread_mutex_t *__mtx);

int pthread_mutex_lock(pthread_mutex_t *__mtx);
int pthread_mutex_trylock(pthread_mutex_t *__mtx);
int pthread_mutex_timedlock(pthread_mutex_t *__restrict __mtx,
		const struct timespec *__restrict __abs_timeout);
int pthread_mutex_unlock(pthread_mutex_t *__mtx);

int pthread_mutex_consistent(pthread_mutex_t *__mtx);

/* ---------------------------------------------------------------------------- */
/* pthread_condattr and pthread_cond functions. */
/* ---------------------------------------------------------------------------- */

int pthread_condattr_init(pthread_condattr_t *__attr);
int pthread_condattr_destroy(pthread_condattr_t *__attr);

int pthread_condattr_getclock(const pthread_condattr_t *__restrict __attr, clockid_t *__restrict __clockid);
int pthread_condattr_setclock(pthread_condattr_t *__attr, clockid_t __clockid);

int pthread_condattr_getpshared(const pthread_condattr_t *__restrict __attr, int *__restrict __pshared);
int pthread_condattr_setpshared(pthread_condattr_t *__attr, int __pshared);

int pthread_cond_init(pthread_cond_t *__restrict __cond, const pthread_condattr_t *__restrict __attr);
int pthread_cond_destroy(pthread_cond_t *__cond);

int pthread_cond_wait(pthread_cond_t *__restrict __cond, pthread_mutex_t *__restrict __mtx);
int pthread_cond_timedwait(pthread_cond_t *__restrict __cond, pthread_mutex_t *__restrict __mtx,
		const struct timespec *__restrict __abs_timeout);
int pthread_cond_signal(pthread_cond_t *__cond);
int pthread_cond_broadcast(pthread_cond_t *__cond);

/* ---------------------------------------------------------------------------- */
/* pthread_barrierattr and pthread_barrier functions. */
/* ---------------------------------------------------------------------------- */

int pthread_barrierattr_init(pthread_barrierattr_t *__attr);
int pthread_barrierattr_destroy(pthread_barrierattr_t *__attr);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *__attr, int __pshared);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict __attr,
		int *__restrict __pshared);

int pthread_barrier_init(pthread_barrier_t *__restrict __barrier, const pthread_barrierattr_t *__restrict __attr,
		unsigned int __count);
int pthread_barrier_destroy(pthread_barrier_t *__barrier);

int pthread_barrier_wait(pthread_barrier_t *__barrier);

/* ---------------------------------------------------------------------------- */
/* pthread_wrlockattr and pthread_rwlock functions. */
/* ---------------------------------------------------------------------------- */

int pthread_rwlockattr_init(pthread_rwlockattr_t *__attr);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *__attr);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *__attr, int __pshared);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict __attr,
		int *__restrict __pshared);

int pthread_rwlock_init(pthread_rwlock_t *__restrict __rwlock, const pthread_rwlockattr_t *__restrict __attr);
int pthread_rwlock_destroy(pthread_rwlock_t *__rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *__rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *__rwlock);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *__rwlock);
int pthread_rwlock_rdlock(pthread_rwlock_t *__rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *__rwlock);

int pthread_getcpuclockid(pthread_t __thrd, clockid_t *__clockid);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _PTHREAD_H */

