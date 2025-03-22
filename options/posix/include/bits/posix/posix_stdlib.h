
#ifndef MLIBC_POSIX_STDLIB_H
#define MLIBC_POSIX_STDLIB_H

#include <bits/posix/locale_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

long random(void);
double drand48(void);
double erand48(unsigned short __s[3]);
unsigned short *seed48(unsigned short __s[3]);
void srand48(long int __seed);
long int mrand48(void);
long jrand48(unsigned short __s[3]);
char *initstate(unsigned int __seed, char *__state, size_t __size);
char *setstate(char *__state);
void srandom(unsigned int __seed);

/* ---------------------------------------------------------------------------- */
/* Environment. */
/* ---------------------------------------------------------------------------- */

int putenv(char *__string);
int setenv(const char *__name, const char *__value, int __overwrite);
int unsetenv(const char *__name);

/* ---------------------------------------------------------------------------- */
/* Path handling. */
/* ---------------------------------------------------------------------------- */

int mkstemp(char *__pattern);
int mkstemps(char *__pattern, int __suffixlen);
int mkostemp(char *__pattern, int __flags);
int mkostemps(char *__pattern, int __suffixlen, int __flags);
char *mkdtemp(char *__path);

char *realpath(const char *__restrict __path, char *__restrict __out);

/* ---------------------------------------------------------------------------- */
/* Pseudoterminals */
/* ---------------------------------------------------------------------------- */

int posix_openpt(int __flags);
int grantpt(int __fd);
int unlockpt(int __fd);
char *ptsname(int __fd);
int ptsname_r(int __fd, char *__buf, size_t __len);

double strtod_l(const char *__restrict__ __nptr, char ** __restrict__ __endptr, locale_t __loc);
long double strtold_l(const char *__restrict__ __nptr, char ** __restrict__ __endptr, locale_t __loc);
float strtof_l(const char *__restrict __string, char **__restrict __end, locale_t __loc);

int getsubopt(char **__restrict__ __optionp, char *const *__restrict__ __tokens, char **__restrict__ __valuep);

/* GNU extension */
char *secure_getenv(const char *__name);
char *canonicalize_file_name(const char *__name);

/* BSD extension */
void *reallocarray(void *__ptr, size_t __count, size_t __size);

int clearenv(void);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* MLIBC_POSIX_STDLIB_H */

