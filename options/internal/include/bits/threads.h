#ifndef _MLIBC_INTERNAL_THREADS_H
#define _MLIBC_INTERNAL_THREADS_H

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

#define __MLIBC_THREAD_ONCE_INITIALIZER {0}

#define __MLIBC_THREAD_DESTRUCTOR_ITERATIONS 8

#define __MLIBC_THREAD_CANCELED ((void*) -1)

#ifdef __cplusplus
extern "C" {
#endif

struct __mlibc_thread_data;

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_INTERNAL_THREADS_H */
