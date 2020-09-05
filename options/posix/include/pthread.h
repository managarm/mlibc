
#ifndef _PTHREAD_H
#define _PTHREAD_H

#include <bits/ansi/clockid_t.h>
// TODO: pthread is not required to define size_t.
#include <bits/size_t.h>
#include <bits/posix/pthread_t.h>

// pthread.h is required to include sched.h and time.h
#include <sched.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

// values for pthread_attr_{get,set}detachstate().
#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1

// Values for pthread_attr_{get,set}scope
#define PTHREAD_SCOPE_SYSTEM 0
#define PTHREAD_SCOPE_PROCESS 1

// values for pthread_{get,set}canceltype().
#define PTHREAD_CANCEL_DEFERRED 0
#define PTHREAD_CANCEL_ASYNCRONOUS 1

// values for pthread_{get,set}cancelstate().
#define PTHREAD_CANCEL_ENABLE 0
#define PTHREAD_CANCEL_DISABLE 1

// values for pthread_mutexattr_{get,set}type().
#define PTHREAD_MUTEX_DEFAULT 0
#define PTHREAD_MUTEX_NORMAL 0
#define PTHREAD_MUTEX_ERRORCHECK 1
#define PTHREAD_MUTEX_RECURSIVE 2

// values for pthread_mutexattr_{get,set}robust().
#define PTHREAD_MUTEX_STALLED 0
#define PTHREAD_MUTEX_ROBUST 1

// values for pthread_mutexattr_{get,set}pshared().
#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED 1

#define PTHREAD_ONCE_INIT {0}
#define PTHREAD_COND_INITIALIZER {0}
#define PTHREAD_MUTEX_INITIALIZER {0, 0, 0}
#define PTHREAD_RWLOCK_INITIALIZER {0, 0, 0}

// TODO: move to own file and include in sys/types.h
struct __mlibc_threadattr {
	// TODO: the guardsize attribute needs to be supported here.

	int __mlibc_deatchstate;
};
typedef struct __mlibc_threadattr pthread_attr_t;

struct __mlibc_key_data;
typedef struct __mlibc_key_data *pthread_key_t;

struct __mlibc_once {
	unsigned int __mlibc_done;
};
typedef struct __mlibc_once pthread_once_t;

struct  __mlibc_mutexattr {
	int __mlibc_type;
	int __mlibc_robust;
};
typedef struct __mlibc_mutexattr pthread_mutexattr_t;

struct __mlibc_mutex {
	unsigned int __mlibc_state;
	unsigned int __mlibc_recursion;
	unsigned int __mlibc_flags;
};
typedef struct __mlibc_mutex pthread_mutex_t;

struct  __mlibc_condattr_struct {
	// TODO: the clock attribute needs to be supported here.
};
typedef struct __mlibc_condattr_struct pthread_condattr_t;

struct  __mlibc_cond {
	// TODO: the clock attribute needs to be supported here.
	unsigned int __mlibc_seq;
};
typedef struct __mlibc_cond pthread_cond_t;

struct  __mlibc_barrierattr_struct {
};
typedef struct __mlibc_barrierattr_struct pthread_barrierattr_t;

struct  __mlibc_barrier {
	unsigned int __mlibc_waiting;
	unsigned int __mlibc_count;
};
typedef struct __mlibc_barrier pthread_barrier_t;

struct __mlibc_fair_rwlock {
	int m;
	int rw;
	unsigned read_count;
};
typedef struct __mlibc_fair_rwlock pthread_rwlock_t;

struct __mlibc_rwlockattr {
	int align;
};
typedef struct __mlibc_rwlockattr pthread_rwlockattr_t;

// ----------------------------------------------------------------------------
// pthread_attr and pthread functions.
// ----------------------------------------------------------------------------

// pthread_attr functions.
int pthread_attr_init(pthread_attr_t *);
int pthread_attr_destroy(pthread_attr_t *);

int pthread_attr_getdetachstate(const pthread_attr_t *, int *);
int pthread_attr_setdetachstate(pthread_attr_t *, int);

int pthread_attr_getstacksize(const pthread_attr_t *__restrict, size_t *__restrict);
int pthread_attr_setstacksize(pthread_attr_t *, size_t);

int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict);
int pthread_attr_setguardsize(pthread_attr_t *, size_t);

int pthread_attr_getscope(const pthread_attr_t *, int);
int pthread_attr_setscope(pthread_attr_t *, int);

// pthread functions.
int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict,
		void *(*) (void *), void *__restrict);
pthread_t pthread_self(void);
int pthread_equal(pthread_t, pthread_t);
int pthread_exit(void *);

int pthread_join(pthread_t, void **);
int pthread_detach(pthread_t);

int pthread_cleanup_push(void (*) (void *), void *);
int pthread_cleanup_pop(int);

int pthread_setname_np(pthread_t, const char *);
int pthread_getname_np(pthread_t, char *, size_t);

int pthread_setcanceltype(int, int *);
int pthread_setcancelstate(int, int *);
void pthread_testcancel(void);
int pthread_cancel(pthread_t);

int pthread_atfork(void (*) (void), void (*) (void), void (*) (void));

// ----------------------------------------------------------------------------
// pthread_key functions.
// ----------------------------------------------------------------------------

int pthread_key_create(pthread_key_t *, void (*) (void *));
int pthread_key_delete(pthread_key_t);

void *pthread_getspecific(pthread_key_t);
int pthread_setspecific(pthread_key_t, const void *);

// ----------------------------------------------------------------------------
// pthread_once functions.
// ----------------------------------------------------------------------------

int pthread_once(pthread_once_t *, void (*) (void));

// ----------------------------------------------------------------------------
// pthread_mutexattr and pthread_mutex functions.
// ----------------------------------------------------------------------------

// pthread_mutexattr functions
int pthread_mutexattr_init(pthread_mutexattr_t *);
int pthread_mutexattr_destroy(pthread_mutexattr_t *);

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict, int *__restrict);
int pthread_mutexattr_settype(pthread_mutexattr_t *, int);

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict, int *__restrict);
int pthread_mutexattr_setrobust(pthread_mutexattr_t *, int);

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);

// pthread_mutex functions
int pthread_mutex_init(pthread_mutex_t *__restrict, const pthread_mutexattr_t *__restrict);
int pthread_mutex_destroy(pthread_mutex_t *);

int pthread_mutex_lock(pthread_mutex_t *);
int pthread_mutex_trylock(pthread_mutex_t *);
int pthread_mutex_timedlock(pthread_mutex_t *__restrict,
		const struct timespec *__restrict);
int pthread_mutex_unlock(pthread_mutex_t *);

int pthread_mutex_consistent(pthread_mutex_t *);

// ----------------------------------------------------------------------------
// pthread_condattr and pthread_cond functions.
// ----------------------------------------------------------------------------

int pthread_condattr_init(pthread_condattr_t *);
int pthread_condattr_destroy(pthread_condattr_t *);

int pthread_condattr_getclock(const pthread_condattr_t *__restrict, clockid_t *__restrict);
int pthread_condattr_setclock(pthread_condattr_t *, clockid_t);

int pthread_condattr_getpshared(const pthread_condattr_t *__restrict, int *__restrict);
int pthread_condattr_setpshared(pthread_condattr_t *, int);

int pthread_cond_init(pthread_cond_t *__restrict, const pthread_condattr_t *__restrict);
int pthread_cond_destroy(pthread_cond_t *);

int pthread_cond_wait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict);
int pthread_cond_timedwait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict,
		const struct timespec *__restrict);
int pthread_cond_signal(pthread_cond_t *);
int pthread_cond_broadcast(pthread_cond_t *);

// ----------------------------------------------------------------------------
// pthread_barrierattr and pthread_barrier functions.
// ----------------------------------------------------------------------------

int pthread_barrierattr_init(pthread_barrierattr_t *);
int pthread_barrierattr_destroy(pthread_barrierattr_t *);

int pthread_barrier_init(pthread_barrier_t *__restrict, const pthread_barrierattr_t *__restrict,
		unsigned int);
int pthread_barrier_destroy(pthread_barrier_t *);

int pthread_barrier_wait(pthread_barrier_t *);

// ----------------------------------------------------------------------------
// pthread_rwlock functions.
// ----------------------------------------------------------------------------

int pthread_rwlock_init(pthread_rwlock_t *__restrict, const pthread_rwlockattr_t *__restrict);
int pthread_rwlock_destroy(pthread_rwlock_t *);
int pthread_rwlock_trywrlock(pthread_rwlock_t *);
int pthread_rwlock_wrlock(pthread_rwlock_t *);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *);
int pthread_rwlock_rdlock(pthread_rwlock_t *);
int pthread_rwlock_unlock(pthread_rwlock_t *);

#ifdef __cplusplus
}
#endif

#endif // _PTHREAD_H

