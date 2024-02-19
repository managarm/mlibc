#ifndef _MLIBC_LIMITS_H
#define _MLIBC_LIMITS_H

#undef MB_LEN_MAX
#define MB_LEN_MAX 4

#define NAME_MAX 255
#define PATH_MAX 4096
#define LINE_MAX 4096
#define PIPE_BUF 4096

#define CHARCLASS_NAME_MAX 14
#define RE_DUP_MAX 255

// This value is a guaranteed minimum, get the current maximum from sysconf
#define NGROUPS_MAX 8
// POSIX states 9 is the minimum for NL_ARGMAX
#define NL_ARGMAX 9

#define SSIZE_MAX __INTPTR_MAX__

#define _POSIX_ARG_MAX 4096
#define _POSIX_OPEN_MAX 16
#define _POSIX_HOST_NAME_MAX 255
#define _POSIX_NAME_MAX 14
#define _POSIX_TZNAME_MAX 6
#define _XOPEN_NAME_MAX 255

#define PTHREAD_STACK_MIN 16384
#define PTHREAD_KEYS_MAX 1024

#include <abi-bits/limits.h>

#if __has_include_next(<limits.h>)
# include_next <limits.h>
#endif

#endif // _MLIBC_LIMITS_H
