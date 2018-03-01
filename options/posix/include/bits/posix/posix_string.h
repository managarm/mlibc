
#ifndef MLIBC_POSIX_STRING_H
#define MLIBC_POSIX_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

char *strdup(const char *string);
char *strndup(const char *, size_t);
size_t strnlen(const char *, size_t);
char *strtok_r(char *__restrict, const char *__restrict, char **__restrict);
char *strsep(char **stringp, const char *delim);
char *strsignal(int sig);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STRING_H

