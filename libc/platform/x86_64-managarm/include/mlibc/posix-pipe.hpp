
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>

extern HelHandle posixPipe;

#endif // MLIBC_POSIX_PIPE

