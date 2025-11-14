
#ifndef _MLIBC_POSIX_STDLIB_H
#define _MLIBC_POSIX_STDLIB_H

#include <bits/posix/locale_t.h>
#include <bits/size_t.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN >= 500
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
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN >= 500 */

/* ---------------------------------------------------------------------------- */
/* Environment. */
/* ---------------------------------------------------------------------------- */

#if defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN
int putenv(char *__string);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_XOPEN */

#if defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2001
int setenv(const char *__name, const char *__value, int __overwrite);
int unsetenv(const char *__name);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2001 */

/* ---------------------------------------------------------------------------- */
/* Path handling. */
/* ---------------------------------------------------------------------------- */

/* Moved from XSI to Base in Issue 7. */
#if defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2008 || __MLIBC_XOPEN < 700
int mkstemp(char *__pattern);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2008 || __MLIBC_XOPEN < 700 */

/* Introduced in Issue 7. */
#if defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2024
char *mkdtemp(char *__path);
#endif

/* Introduced in Issue 8, previously a GNU extension. */
#if defined(_GNU_SOURCE) || __MLIBC_POSIX2024
int mkostemp(char *__pattern, int __flags);
#endif

#if defined(_DEFAULT_SOURCE)
int mkstemps(char *__pattern, int __suffixlen);
#endif /* defined(_DEFAULT_SOURCE) */

#if defined(_GNU_SOURCE)
int mkostemps(char *__pattern, int __suffixlen, int __flags);
#endif /* defined(_GNU_SOURCE) */

#if defined(_DEFAULT_SOURCE) || (defined(__MLIBC_XOPEN) && __MLIBC_XOPEN < 800) || __MLIBC_POSIX2024
char *realpath(const char *__restrict __path, char *__restrict __out);
#endif /* defined(_DEFAULT_SOURCE) || (defined(__MLIBC_XOPEN) && __MLIBC_XOPEN < 800) || __MLIBC_POSIX2024 */

/* ---------------------------------------------------------------------------- */
/* Pseudoterminals */
/* ---------------------------------------------------------------------------- */

#if __MLIBC_XOPEN >= 600
int posix_openpt(int __flags);
#endif /* __MLIBC_XOPEN >= 600 */

#if __MLIBC_XOPEN >= 500
int grantpt(int __fd);
int unlockpt(int __fd);
char *ptsname(int __fd);
#endif /* __MLIBC_XOPEN >= 500 */

#if defined(_GNU_SOURCE) || __MLIBC_XOPEN >= 800
int ptsname_r(int __fd, char *__buf, size_t __len);
#endif /* defined(_GNU_SOURCE) || __MLIBC_XOPEN >= 800 */

double strtod_l(const char *__restrict__ __nptr, char ** __restrict__ __endptr, locale_t __loc);
long double strtold_l(const char *__restrict__ __nptr, char ** __restrict__ __endptr, locale_t __loc);
float strtof_l(const char *__restrict __string, char **__restrict __end, locale_t __loc);

int getsubopt(char **__restrict__ __optionp, char *const *__restrict__ __tokens, char **__restrict__ __valuep);

#if defined(_GNU_SOURCE) || __MLIBC_POSIX2024
char *secure_getenv(const char *__name);
#endif /* defined(_GNU_SOURCE) || __MLIBC_POSIX2024 */

#if defined(_GNU_SOURCE)
char *canonicalize_file_name(const char *__name);
#endif /* defined(_GNU_SOURCE) */

#if defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2024
void *reallocarray(void *__ptr, size_t __count, size_t __size);
#endif /* defined(_DEFAULT_SOURCE) || __MLIBC_POSIX2024 */

#if defined(_DEFAULT_SOURCE)
int clearenv(void);
#endif /* defined(_DEFAULT_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_POSIX_STDLIB_H */

