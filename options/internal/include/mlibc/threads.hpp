#pragma once

#include <abi-bits/clockid_t.h>
#include <bits/ansi/timespec.h>
#include <bits/threads.h>

namespace mlibc {

int thread_once(__mlibc_once *once, void (*func) (void));

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int);
int thread_attr_init(struct __mlibc_threadattr *attr);
int thread_join(struct __mlibc_thread_data *thread, void *res);
int thread_detach(struct __mlibc_thread_data *thread);

union thread_exit_return {
	void *voidPtr;
	int integer;
};
__attribute__ ((__noreturn__)) void thread_exit(thread_exit_return ret_val);

int thread_mutex_init(struct __mlibc_mutex *__restrict mutex, const struct __mlibc_mutexattr *__restrict attr);
int thread_mutex_destroy(struct __mlibc_mutex *mutex);
int thread_mutex_lock(struct __mlibc_mutex *mutex);
int thread_mutex_timedlock(struct __mlibc_mutex *mutex, const struct timespec *__restrict abstime, clockid_t __clockid);
int thread_mutex_trylock(struct __mlibc_mutex *mutex);
int thread_mutex_unlock(struct __mlibc_mutex *mutex);

int thread_mutexattr_init(struct __mlibc_mutexattr *attr);
int thread_mutexattr_destroy(struct __mlibc_mutexattr *attr);
int thread_mutexattr_gettype(const struct __mlibc_mutexattr *__restrict attr, int *__restrict type);
int thread_mutexattr_settype(struct __mlibc_mutexattr *attr, int type);

int thread_cond_init(struct __mlibc_cond *__restrict cond, const struct __mlibc_condattr *__restrict attr);
int thread_cond_destroy(struct __mlibc_cond *cond);
int thread_cond_broadcast(struct __mlibc_cond *cond);
int thread_cond_timedwait(struct __mlibc_cond *__restrict cond, __mlibc_mutex *__restrict mutex, const struct timespec *__restrict abstime, clockid_t __clockid);

int thread_key_create(__mlibc_uintptr *out, void (*destructor)(void *));
int thread_key_delete(__mlibc_uintptr key);
void *thread_key_get(__mlibc_uintptr key);
int thread_key_set(__mlibc_uintptr key, const void *value);

} // namespace mlibc
