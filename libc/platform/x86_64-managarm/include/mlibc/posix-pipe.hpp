
#ifndef MLIBC_POSIX_PIPE
#define MLIBC_POSIX_PIPE

// FIXME: required for hel.h
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#pragma GCC visibility push(hidden)

#include <frigg/initializer.hpp>

#include <hel.h>
#include <hel-syscalls.h>
#include <helx.hpp>

extern frigg::LazyInitializer<helx::EventHub> eventHub;
extern frigg::LazyInitializer<helx::Pipe> posixPipe;

int64_t allocPosixRequest();

#pragma GCC visibility pop

#endif // MLIBC_POSIX_PIPE

