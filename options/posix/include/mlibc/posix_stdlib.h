
#ifndef MLIBC_POSIX_STDLIB_H
#define MLIBC_POSIX_STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

long random(void);

int putenv(const char *);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STDLIB_H

