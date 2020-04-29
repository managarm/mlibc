#ifndef _LIMITS_H
#define _LIMITS_H

#define CHAR_BIT 8
#define LONG_BIT (CHAR_BIT * sizeof(long))

#undef LLONG_MAX
#undef ULLONG_MAX
#define LLONG_MIN (-__LONG_LONG_MAX__ - 1LL)
#define LLONG_MAX __LONG_LONG_MAX__
#define ULLONG_MAX (__LONG_LONG_MAX__ * 2ULL + 1ULL)

#define NAME_MAX 255
#define PATH_MAX 4096
#define LINE_MAX 4096
#define PIPE_BUF 4096

#if INTPTR_MAX == INT64_MAX
# define SSIZE_MAX LONG_MAX
#elif INTPTR_MAX == INT32_MAX
# define SSIZE_MAX INT_MAX
#endif

#define _POSIX_ARG_MAX 4096

#endif // _LIMITS_H
