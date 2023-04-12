#pragma once

#include <bits/threads.h>

namespace mlibc {

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int);
int thread_attr_init(struct __mlibc_threadattr *attr);

}
