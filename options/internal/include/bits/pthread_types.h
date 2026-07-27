#ifndef _MLIBC_INTERNAL_PTHREAD_TYPES_H
#define _MLIBC_INTERNAL_PTHREAD_TYPES_H

#include <bits/types.h>
#include <bits/pthread_attr_t.h>
#include <bits/pthread_sizes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	char __opaque[__MLIBC_THREAD_BARRIER_SIZE];
} __attribute__((aligned(__INTPTR_WIDTH__ / 8))) pthread_barrier_t;

typedef struct {
	void *__heap_ptr;
} pthread_barrierattr_t;

typedef struct {
	char __opaque[__MLIBC_THREAD_COND_SIZE];
} __attribute__((aligned(__INTPTR_WIDTH__ / 8))) pthread_cond_t;

typedef struct {
	void *__heap_ptr;
} pthread_condattr_t;

typedef __mlibc_uintptr pthread_key_t;

typedef struct {
	char __opaque[__MLIBC_THREAD_MUTEX_SIZE];
} __attribute__((aligned(__INTPTR_WIDTH__ / 8))) pthread_mutex_t;

typedef struct {
	void *__heap_ptr;
} pthread_mutexattr_t;

typedef struct {
	char __opaque[__MLIBC_THREAD_RWLOCK_SIZE];
} __attribute__((aligned(__INTPTR_WIDTH__ / 8))) pthread_rwlock_t;

typedef struct {
	void *__heap_ptr;
} pthread_rwlockattr_t;

typedef struct {
	int __opaque[__MLIBC_THREAD_ONCE_SIZE / sizeof(int)];
} pthread_once_t;

typedef struct {
	int __opaque[__MLIBC_THREAD_SPINLOCK_SIZE / sizeof(int)];
} pthread_spinlock_t;

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_INTERNAL_PTHREAD_TYPES_H */
