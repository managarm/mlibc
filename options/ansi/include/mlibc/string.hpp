#pragma once

#include <stddef.h>

using strcmp_signature = int (*)(const char*, const char*);
using strlen_signature = size_t (*)(const char *s);

extern "C" {

[[gnu::visibility("protected")]] int __mlibc_strcmp_default(const char *a, const char *b);
[[gnu::visibility("protected")]] size_t __mlibc_strlen_default(const char *s);

}
