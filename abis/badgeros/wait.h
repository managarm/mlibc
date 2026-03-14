#ifndef _ABIBITS_WAIT_H
#define _ABIBITS_WAIT_H

#include <mlibc-config.h>

#define WNOHANG 1
#define WUNTRACED 2
#define WSTOPPED 2
#define WEXITED 4
#define WCONTINUED 8
#define WNOWAIT 0x01000000

#define __WCOREFLAG 0x80

#define WEXITSTATUS(x) (((x) & 0xff00) >> 8)
#define WTERMSIG(x) (((x) & 0xff00) >> 8)
#define WSTOPSIG(x) (((x) & 0xff00) >> 8)

/* Whether the child exited normally (by means of `SYSCALL_PROC_EXIT`). */
#define WIFEXITED(x) (((x) & 0xff) == 0)
/* Whether the child was killed by a signal. */
#define WIFSIGNALED(x) ((x) & 0x40)
/* Whether the child was suspended by a signal. */
#define WIFSTOPPED(x) ((x) & 0x20)
/* Whether the child was resumed by `SIGCONT`. */
#define WIFCONTINUED(x) ((x) & 0x10)

#if defined(_DEFAULT_SOURCE)
#define WCOREFLAG __WCOREFLAG
#endif

#if defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2024
/* Whether the child dumped core. */
#define WCOREDUMP(x) ((x) & __WCOREFLAG)
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2024 */

/* glibc extension, but also useful for kernels */
#if defined(_DEFAULT_SOURCE)
#define W_EXITCODE(ret, sig) (((ret) << 8) | (sig))
#endif /* defined(_DEFAULT_SOURCE) */

#endif /*_ABIBITS_WAIT_H */
