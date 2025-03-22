#ifndef _STDLIB_H
#define _STDLIB_H

#include <alloca.h>
#include <mlibc-config.h>
#include <abi-bits/wait.h>
#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

/* [7.22] General utilities */

typedef struct {
	int quot, rem;
} div_t;

typedef struct {
	long quot, rem;
} ldiv_t;

typedef struct {
	long long quot, rem;
} lldiv_t;

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX 0x7FFFFFFF

/* TODO: this should not be a compile-time constant */
#define MB_CUR_MAX ((size_t)4)

#ifndef __MLIBC_ABI_ONLY

/* [7.22.1] Numeric conversion functions */

double atof(const char *__string);
int atoi(const char *__string);
long atol(const char *__string);
long long atoll(const char *__string);
double strtod(const char *__restrict __string, char **__restrict __end);
float strtof(const char *__restrict __string, char **__restrict __end);
long double strtold(const char *__restrict __string, char **__restrict __end);
long strtol(const char *__restrict __string, char **__restrict __end, int __base);
long long strtoll(const char *__restrict __string, char **__restrict __end, int __base);
unsigned long strtoul(const char *__restrict __string, char **__restrict __end, int __base);
unsigned long long strtoull(const char *__restrict __string, char **__restrict __end, int __base);

/* [7.22.2] Pseudo-random sequence generation functions */

int rand(void);
int rand_r(unsigned *__seed);
void srand(unsigned int __seed);

/* [7.22.3] Memory management functions */

void *aligned_alloc(size_t __alignment, size_t __size);
void *calloc(size_t __count, size_t __size);
void free(void *__pointer);
void *malloc(size_t __size);
void *realloc(void *__pointer, size_t __size);

int posix_memalign(void **__out, size_t __alignment, size_t __size);

/* [7.22.4] Communication with the environment */

__attribute__((__noreturn__)) void abort(void);
int atexit(void (*__func)(void));
int at_quick_exit(void (*__func)(void));
__attribute__((__noreturn__)) void exit(int __status);
__attribute__((__noreturn__)) void _Exit(int __status);
char *getenv(const char *__name);
__attribute__((__noreturn__)) void quick_exit(int __status);
int system(const char *__string);

/* GLIBC extension. */
char *mktemp(char *__pattern);

/* [7.22.5] Searching and sorting utilities */

void *bsearch(const void *__key, const void *__base, size_t __count, size_t __size,
		int (*__compare)(const void *__a, const void *__b));
void qsort(void *__base, size_t __count, size_t __size,
		int (*__compare)(const void *__a, const void *__b));
void qsort_r(void *__base, size_t __nmemb, size_t __size,
		int (*__compar)(const void *__a, const void *__b, void *__arg),
		void *__arg);

/* [7.22.6] Integer arithmetic functions */

int abs(int __number);
long labs(long __number);
long long llabs(long long __number);

div_t div(int __number, int __denom);
ldiv_t ldiv(long __number, long __denom);
lldiv_t lldiv(long long __number, long long __denom);

/* [7.22.7] Multibyte character conversion functions */

int mblen(const char *__mbs, size_t __limit);
int mbtowc(wchar_t *__restrict __wc, const char *__restrict __mb_chr, size_t __max_size);
int wctomb(char *__mb_chr, wchar_t __wc);

/* [7.22.8] Multibyte string conversion functions */

size_t mbstowcs(wchar_t *__restrict __wc_string, const char *__restrict __mb_string, size_t __max_size);
size_t wcstombs(char *__restrict __mb_string, const wchar_t *__restrict __wc_string, size_t __max_size);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_BSD_OPTION
#	include <bits/bsd/bsd_stdlib.h>
#endif
#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_stdlib.h>
#endif
#if __MLIBC_GLIBC_OPTION
#	include <bits/glibc/glibc_stdlib.h>
#endif

#endif /* _STDLIB_H */

