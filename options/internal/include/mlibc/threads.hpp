#pragma once

#include <bits/threads.h>

namespace mlibc {

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int);
int thread_attr_init(struct __mlibc_threadattr *attr);
int thread_join(struct __mlibc_thread_data *thread, void *res);

int thread_mutex_destroy(struct __mlibc_mutex *mutex);

int thread_mutexattr_init(struct __mlibc_mutexattr *attr);
int thread_mutexattr_destroy(struct __mlibc_mutexattr *attr);
int thread_mutexattr_gettype(const struct __mlibc_mutexattr *__restrict attr, int *__restrict type);
int thread_mutexattr_settype(struct __mlibc_mutexattr *attr, int type);

int thread_cond_init(struct __mlibc_cond *__restrict cond, const struct __mlibc_condattr *__restrict attr);
int thread_cond_destroy(struct __mlibc_cond *cond);
int thread_cond_broadcast(struct __mlibc_cond *cond);

}
