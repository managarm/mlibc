
#ifndef MLIBC_POSIX_STDLIB_H
#define MLIBC_POSIX_STDLIB_H

#include <bits/posix/locale_t.h>

#ifdef __cplusplus
extern "C" {
#endif

long random(void);
double drand48(void);
void srand48(long int);
char *initstate(unsigned int, char *, size_t);
char *setstate(char *);
void srandom(unsigned int);

// ----------------------------------------------------------------------------
// Environment.
// ----------------------------------------------------------------------------

int putenv(char *);
int setenv(const char *, const char *, int);
int unsetenv(const char *);

// ----------------------------------------------------------------------------
// Path handling.
// ----------------------------------------------------------------------------

int mkstemp(char *);
int mkostemp(char *, int flags);
char *mkdtemp(char *path);

char *realpath(const char *__restrict, char *__restrict);

// ----------------------------------------------------------------------------
// Pseudoterminals
// ----------------------------------------------------------------------------

int posix_openpt(int flags);
int grantpt(int fd);
int unlockpt(int fd);
char *ptsname(int fd);
int ptsname_r(int fd, char *buf, size_t len);

double strtod_l(const char *__restrict__ nptr, char ** __restrict__ endptr, locale_t loc);

#ifdef __cplusplus
}
#endif

#endif // MLIBC_POSIX_STDLIB_H

