#ifndef _MLIBC_POSIX_SYS_TIME_H
#define _MLIBC_POSIX_SYS_TIME_H

#include <bits/posix/timeval.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
int utimes(const char *__filename, const struct timeval __tv[2]);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN */

/* Not standardized, Linux and BSDs have it */
#if defined(_DEFAULT_SOURCE)
int futimes(int __fd, const struct timeval __tv[2]);
int lutimes(const char *__filename, const struct timeval __tv[2]);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_POSIX_SYS_TIME_H */
