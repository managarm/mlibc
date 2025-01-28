#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT 8

#ifndef MB_LEN_MAX
# define MB_LEN_MAX 4
#endif

#ifdef LONG_MAX
# ifdef LONG_MAX == INT32_MAX
#  define LONG_BIT 32
# else
/* Safe assumption */
#  define LONG_BIT 64
# endif
#elif defined __LONG_MAX__
# if __LONG_MAX__ == INT32_MAX
#  define LONG_BIT 32
# else
/* Safe assumption */
#  define LONG_BIT 64
# endif
#else
# error "Unsupported configuration, please define either LONG_MAX or __LONG_MAX__"
#endif

#undef SCHAR_MIN
#undef SCHAR_MAX
#undef CHAR_MIN
#undef CHAR_MAX
#undef UCHAR_MAX
#undef SHRT_MIN
#undef SHRT_MAX
#undef USHRT_MAX
#undef INT_MIN
#undef INT_MAX
#undef UINT_MAX
#undef LONG_MIN
#undef LONG_MAX
#undef ULONG_MAX
#undef LLONG_MIN
#undef LLONG_MAX
#undef ULLONG_MAX

#define SCHAR_MIN (-__SCHAR_MAX__ - 1)
#define SCHAR_MAX __SCHAR_MAX__
#if __SCHAR_MAX__ == __INT_MAX__
# define UCHAR_MAX (__SCHAR_MAX__ * 2U + 1U)
#else
# define UCHAR_MAX (__SCHAR_MAX__ * 2 + 1)
#endif

#ifdef __CHAR_UNSIGNED__
# define CHAR_MAX UCHAR_MAX
# if __SCHAR_MAX__ == __INT_MAX__
#  define CHAR_MIN 0U
# else
#  define CHAR_MIN 0
# endif
#else
# define CHAR_MAX SCHAR_MAX
# define CHAR_MIN SCHAR_MIN
#endif

#define SHRT_MIN (-__SHRT_MAX__ - 1)
#define SHRT_MAX __SHRT_MAX__
#if __SHRT_MAX__ == __INT_MAX__
# define USHRT_MAX (__SHRT_MAX__ * 2U + 1U)
#else
# define USHRT_MAX (__SHRT_MAX__ * 2 + 1)
#endif

#define INT_MIN (-__INT_MAX__ - 1)
#define INT_MAX __INT_MAX__
#define UINT_MAX (__INT_MAX__ * 2U + 1U)

#define LONG_MIN (-__LONG_MAX__ - 1L)
#define LONG_MAX __LONG_MAX__
#define ULONG_MAX (__LONG_MAX__ * 2UL + 1UL)

#define LLONG_MIN (-__LONG_LONG_MAX__ - 1LL)
#define LLONG_MAX __LONG_LONG_MAX__
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)

#define NAME_MAX 255
#define PATH_MAX 4096
#define LINE_MAX 4096
#define PIPE_BUF 4096

#define CHARCLASS_NAME_MAX 14
#define RE_DUP_MAX 255

/* This value is a guaranteed minimum, get the current maximum from sysconf */
#define NGROUPS_MAX 8
/* POSIX states 9 is the minimum for NL_ARGMAX */
#define NL_ARGMAX 9

#if INTPTR_MAX == INT64_MAX
# define SSIZE_MAX LONG_MAX
#elif INTPTR_MAX == INT32_MAX
# define SSIZE_MAX INT_MAX
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

#endif /* _LIMITS_H */
