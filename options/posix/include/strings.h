
#ifndef _STRINGS_H
#define _STRINGS_H

#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *index (const char *s, int c);
char *rindex(const char *s, int c);

int ffs(int word);
int strcasecmp(const char *a, const char *b);
int strncasecmp(const char *a, const char *b, size_t size);

/* Marked as obsolete in posix 2008 but used by at least tracker */
int bcmp(const void *s1, const void *s2, size_t n);
void bcopy(const void *s1, void *s2, size_t n);
void bzero(void *s, size_t n);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _STRINGS_H

