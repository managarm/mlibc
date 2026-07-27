#pragma once

using strcmp_signature = int (*)(const char*, const char*);

extern "C" {

[[gnu::visibility("protected")]] int __mlibc_strcmp_default(const char *a, const char *b);

}
