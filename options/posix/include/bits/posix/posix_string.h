
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

int strcoll_l(const char *s1, const char *s2, locale_t locale);

// GNU extensions.
char *strcasestr(const char *, const char *);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STRING_H

