
#ifndef MLIBC_POSIX_STRING_H
#define MLIBC_POSIX_STRING_H

#include <alloca.h>
#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

char *strdup(const char *string);
char *strndup(const char *, size_t);
size_t strnlen(const char *, size_t);
char *strtok_r(char *__restrict, const char *__restrict, char **__restrict);
char *strsep(char **stringp, const char *delim);
char *strsignal(int sig);
char *stpcpy(char *__restrict, const char *__restrict);
char *stpncpy(char *__restrict, const char *__restrict, size_t n);
void *memccpy(void *__restrict dest, const void *__restrict src, int c, size_t n);

int strcoll_l(const char *s1, const char *s2, locale_t locale);

// GNU extensions.
#if defined(_GNU_SOURCE)
char *strcasestr(const char *, const char *);
#define strdupa(x) ({ \
	const char *str = (x); \
	size_t len = strlen(str) + 1; \
	char *buf = alloca(len); \
	(char *) memcpy(buf, str, len); \
})
#define strndupa(x, y) ({ \
	const char *str = (x); \
	size_t len = strnlen(str, (y)) + 1; \
	char *buf = alloca(len); \
	buf[len - 1] = '\0'; \
	(char *) memcpy(buf, str, len - 1); \
})
void *memrchr(const void *, int, size_t);
#endif /* defined(_GNU_SOURCE) */

// BSD extensions
size_t strlcpy(char *d, const char *s, size_t n);
size_t strlcat(char *d, const char *s, size_t n);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STRING_H

