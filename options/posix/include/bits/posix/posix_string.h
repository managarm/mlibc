
#ifndef MLIBC_POSIX_STRING_H
#define MLIBC_POSIX_STRING_H

#include <alloca.h>
#include <bits/posix/locale_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *strdup(const char *__string);
char *strndup(const char *__string, size_t __max_size);
size_t strnlen(const char *__s, size_t __max_size);
char *strtok_r(char *__restrict __s, const char *__restrict __delim, char **__restrict __ptr);
char *strsep(char **__stringp, const char *__delim);
char *strsignal(int __sig);
char *stpcpy(char *__restrict __dest, const char *__restrict __src);
char *stpncpy(char *__restrict __dest, const char *__restrict __src, size_t __n);
void *memccpy(void *__restrict __dest, const void *__restrict __src, int __c, size_t __n);

int strcoll_l(const char *__s1, const char *__s2, locale_t __locale);

char *strerror_l(int __errnum, locale_t __locale);

/* GNU extensions. */
#if defined(_GNU_SOURCE)
char *strcasestr(const char *__s1, const char *__s2);
#define strdupa(x) ({ \
	const char *__str = (x); \
	size_t __len = strlen(__str) + 1; \
	char *__buf = alloca(__len); \
	(char *) memcpy(__buf, __str, __len); \
})
#define strndupa(x, y) ({ \
	const char *__str = (x); \
	size_t __len = strnlen(__str, (y)) + 1; \
	char *__buf = alloca(__len); \
	__buf[__len - 1] = '\0'; \
	(char *) memcpy(__buf, __str, __len - 1); \
})
void *memrchr(const void *__m, int __c, size_t __n);
#endif /* defined(_GNU_SOURCE) */

/* BSD extensions */
size_t strlcpy(char *__d, const char *__s, size_t __n);
size_t strlcat(char *__d, const char *__s, size_t __n);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_STRING_H */

