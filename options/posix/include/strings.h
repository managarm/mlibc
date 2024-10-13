
#ifndef _STRINGS_H
#define _STRINGS_H

#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *index (const char *__s, int __c);
char *rindex(const char *__s, int __c);

int ffs(int __word);
int strcasecmp(const char *__a, const char *__b);
int strncasecmp(const char *__a, const char *__b, size_t __size);

/* Marked as obsolete in posix 2008 but used by at least tracker */
int bcmp(const void *__s1, const void *__s2, size_t __n);
void bcopy(const void *__s1, void *__s2, size_t __n);
void bzero(void *__s, size_t __n);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _STRINGS_H */

