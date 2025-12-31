#ifndef _LIMITS_H
#define _LIMITS_H

#include <mlibc-config.h>

#define CHAR_BIT 8

#ifndef MB_LEN_MAX
# define MB_LEN_MAX 4
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

#define PATH_MAX 4096
#define LINE_MAX 4096
#define PIPE_BUF 4096

#define FILESIZEBITS 64

#define CHARCLASS_NAME_MAX 14
#define COLL_WEIGHTS_MAX 255
#define GETENTROPY_MAX 256
#define RE_DUP_MAX 255

#if !defined(NGROUPS_MAX)
/* This value is a guaranteed minimum, get the current maximum from sysconf */
#define NGROUPS_MAX 8
#endif /* !defined(NGROUPS_MAX) */

#if __INTPTR_MAX__ == __INT64_MAX__
# define SSIZE_MAX LONG_MAX
#elif __INTPTR_MAX__ == __INT32_MAX__
# define SSIZE_MAX INT_MAX
#endif

#if __LONG_MAX__ == __INT32_MAX__
# define LONG_BIT 32
#else
/* Safe assumption */
# define LONG_BIT 64
#endif

#define WORD_BIT 32

#define _POSIX_AIO_LISTIO_MAX 2
#define _POSIX_AIO_MAX 1
#define _POSIX_ARG_MAX 4096
#define _POSIX_CHILD_MAX 25
#define _POSIX_CLOCKRES_MIN 20000000
#define _POSIX_DELAYTIMER_MAX 32
#define _POSIX_HOST_NAME_MAX 255
#define _POSIX_LINK_MAX 8
#define _POSIX_LOGIN_NAME_MAX 9
#define _POSIX_MAX_CANON 255
#define _POSIX_MAX_INPUT 255
#define _POSIX_MQ_OPEN_MAX 8
#define _POSIX_MQ_PRIO_MAX 32
#define _POSIX_NAME_MAX 14
#define _POSIX_NGROUPS_MAX 8
#define _POSIX_OPEN_MAX 20
#define _POSIX_PATH_MAX 256
#define _POSIX_PIPE_BUF 512
#define _POSIX_RE_DUP_MAX 255
#define _POSIX_RTSIG_MAX 8
#define _POSIX_SEM_NSEMS_MAX 256
#define _POSIX_SEM_VALUE_MAX 32767
#define _POSIX_SIGQUEUE_MAX 32
#define _POSIX_SSIZE_MAX 32767
#define _POSIX_SS_REPL_MAX 4
#define _POSIX_STREAM_MAX 8
#define _POSIX_SYMLINK_MAX 255
#define _POSIX_SYMLOOP_MAX 8
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS 4
#define _POSIX_THREAD_KEYS_MAX 128
#define _POSIX_THREAD_THREADS_MAX 64
#define _POSIX_TIMER_MAX 32
#define _POSIX_TTY_NAME_MAX 9
#define _POSIX_TZNAME_MAX 6
#define _POSIX2_BC_BASE_MAX 99
#define _POSIX2_BC_DIM_MAX 2048
#define _POSIX2_BC_SCALE_MAX 99
#define _POSIX2_BC_STRING_MAX 1000
#define _POSIX2_CHARCLASS_NAME_MAX 14
#define _POSIX2_COLL_WEIGHTS_MAX 2
#define _POSIX2_EXPR_NEST_MAX 32
#define _POSIX2_LINE_MAX 2048
#define _POSIX2_RE_DUP_MAX 255

#if __MLIBC_XOPEN
#define _XOPEN_IOV_MAX 16
#define _XOPEN_NAME_MAX 255
#define _XOPEN_PATH_MAX 1024
#endif /* __MLIBC_XOPEN */

#define _POSIX2_LINE_MAX 2048

#define BC_BASE_MAX _POSIX2_BC_BASE_MAX
#define BC_DIM_MAX _POSIX2_BC_DIM_MAX
#define BC_SCALE_MAX _POSIX2_BC_SCALE_MAX
#define BC_STRING_MAX _POSIX2_BC_STRING_MAX
#define EXPR_NEST_MAX _POSIX2_EXPR_NEST_MAX
/* This value is a guaranteed minimum, get the current maximum from sysconf */
#define TZNAME_MAX _POSIX_TZNAME_MAX

/* POSIX states 9 is the minimum for NL_ARGMAX */
#define NL_ARGMAX 9
#define NL_MSGMAX INT_MAX
#define NL_SETMAX INT_MAX
#define NL_TEXTMAX INT_MAX
#if __MLIBC_XOPEN
#define NL_LANGMAX _POSIX2_LINE_MAX
#endif /* __MLIBC_XOPEN */

#define PTHREAD_STACK_MIN 16384
#define PTHREAD_KEYS_MAX 1024

#include <abi-bits/limits.h>

#if __MLIBC_POSIX_OPTION
#include <abi-bits/sig-limits.h>
#endif

#define IOV_MAX __MLIBC_IOV_MAX
#define LOGIN_NAME_MAX __MLIBC_LOGIN_NAME_MAX
#define HOST_NAME_MAX __MLIBC_HOST_NAME_MAX
#ifndef NAME_MAX
#define NAME_MAX __MLIBC_NAME_MAX
#endif
#define OPEN_MAX __MLIBC_OPEN_MAX

#endif /* _LIMITS_H */
