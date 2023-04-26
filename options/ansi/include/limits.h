#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT 8

#ifdef LONG_MAX
# ifdef LONG_MAX == INT32_MAX
#  define LONG_BIT 32
# else
// Safe assumption
#  define LONG_BIT 64
# endif
#elif defined __LONG_MAX__
# if __LONG_MAX__ == INT32_MAX
#  define LONG_BIT 32
# else
// Safe assumption
#  define LONG_BIT 64
# endif
#else
# error "Unsupported configuration, please define either LONG_MAX or __LONG_MAX__"
#endif

#undef LLONG_MAX
#undef ULLONG_MAX
#define LLONG_MIN (-__LONG_LONG_MAX__ - 1LL)
#define LLONG_MAX __LONG_LONG_MAX__
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)

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

#if INTPTR_MAX == INT64_MAX
# define SSIZE_MAX LONG_MAX
#elif INTPTR_MAX == INT32_MAX
# define SSIZE_MAX INT_MAX
#endif

#ifndef INT_MAX 
# define INT_MAX 0xfffffff
# define INT_MIN (-INT_MAX -1)
#elif ! defined(LONG_MAX)
# if defined(__x86_64__)
#  define LONG_MAX 0x7fffffffffffffffL
#  define LONG_MIN (-LONG_MAX-1)
# endif
#endif

#ifndef UCHAR_MAX 
#define UCHAR_MAX 255
#endif

#define UINT_MAX 0xffffffffU
#define ULONG_MAX (2UL*LONG_MAX+1)

#ifndef MB_LEN_MAX
# define MB_LEN_MAX 4 
#endif

#define _POSIX_ARG_MAX 4096
#define _POSIX_OPEN_MAX 16
#define _POSIX_HOST_NAME_MAX 255
#define _POSIX_NAME_MAX 14
#define _POSIX_TZNAME_MAX 6
#define _XOPEN_NAME_MAX 255

#define PTHREAD_STACK_MIN 16384
#define PTHREAD_KEYS_MAX 1024

#include <abi-bits/limits.h>

#endif // _LIMITS_H
