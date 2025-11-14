
#ifndef _FNMATCH_H
#define _FNMATCH_H

#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

/* POSIX-defined fnmatch() flags. */
#define FNM_PATHNAME 0x1
#define FNM_NOESCAPE 0x2
#define FNM_PERIOD 0x4

#if defined(_GNU_SOURCE) || __MLIBC_POSIX2024
#define FNM_CASEFOLD 0x10
#define FNM_IGNORECASE FNM_CASEFOLD
#endif /* defined(_GNU_SOURCE) || __MLIBC_POSIX2024 */

/* GNU extensions for fnmatch() flags. */
#if defined(_GNU_SOURCE)
#define FNM_LEADING_DIR 0x8
#define FNM_EXTMATCH 0x20
#endif /* defined(_GNU_SOURCE) */

/* fnmatch() return values. */
#define FNM_NOMATCH 1

#ifndef __MLIBC_ABI_ONLY

int fnmatch(const char *__pattern, const char *__string, int __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _FNMATCH_H */

