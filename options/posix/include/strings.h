
#ifndef _STRINGS_H
#define _STRINGS_H

#include <bits/posix/locale_t.h>
#include <bits/size_t.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2008) || __MLIBC_XOPEN >= 700
int ffs(int __word);
#endif

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN >= 800
int ffsl(long __i);
int ffsll(long long __i);
#endif

int strcasecmp(const char *__a, const char *__b);
int strncasecmp(const char *__a, const char *__b, size_t __size);

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2008)
/* Marked as obsolete in POSIX 2008 but used by at least tracker */
int bcmp(const void *__s1, const void *__s2, size_t __n);
void bcopy(const void *__s1, void *__s2, size_t __n);
void bzero(void *__s, size_t __n);

char *index (const char *__s, int __c);
char *rindex(const char *__s, int __c);
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _STRINGS_H */

