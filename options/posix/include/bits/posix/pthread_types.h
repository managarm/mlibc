#ifndef _MLIBC_POSIX_BITS_PTHREAD_TYPTES_HPP
#define _MLIBC_POSIX_BITS_PTHREAD_TYPTES_HPP

#include <bits/types.h>
#include <bits/posix/pthread_attr_t.h>

typedef struct __mlibc_barrier pthread_barrier_t;
typedef struct __mlibc_barrierattr_struct pthread_barrierattr_t;
typedef struct __mlibc_cond pthread_cond_t;
typedef struct __mlibc_condattr pthread_condattr_t;
typedef __mlibc_uintptr pthread_key_t;
typedef struct __mlibc_mutex pthread_mutex_t;
typedef struct __mlibc_mutexattr pthread_mutexattr_t;
typedef struct __mlibc_once pthread_once_t;
typedef struct __mlibc_fair_rwlock pthread_rwlock_t;
typedef struct __mlibc_rwlockattr pthread_rwlockattr_t;
typedef struct __mlibc_spinlock pthread_spinlock_t;

#endif /* _MLIBC_POSIX_BITS_PTHREAD_TYPTES_HPP */
