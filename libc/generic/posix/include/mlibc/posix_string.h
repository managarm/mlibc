
#ifndef MLIBC_POSIX_STRING_H
#define MLIBC_POSIX_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

char *strdup(const char *string);
char *strndup(const char *, size_t);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STRING_H

