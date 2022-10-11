
#ifndef MLIBC_POSIX_STRING_H
#define MLIBC_POSIX_STRING_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

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
char *strcasestr(const char *, const char *);
char *strdupa(const char *);
char *strndupa(const char *, size_t);
void *memrchr(const void *, int, size_t);

// BSD extensions
size_t strlcpy(char *d, const char *s, size_t n);
size_t strlcat(char *d, const char *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STRING_H

