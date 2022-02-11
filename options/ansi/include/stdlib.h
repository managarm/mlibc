#ifndef _STDLIB_H
#define _STDLIB_H

#include <alloca.h>
#include <mlibc-config.h>
#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>

#ifdef __cplusplus
extern "C" {
#endif

// [7.22] General utilities

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

// TODO: this should not be a compile-time constant
#define MB_CUR_MAX 4

// [7.22.1] Numeric conversion functions

double atof(const char *string);
int atoi(const char *string);
long atol(const char *string);
long long atoll(const char *string);
double strtod(const char *__restrict string, char **__restrict end);
float strtof(const char *__restrict string, char **__restrict end);
long double strtold(const char *__restrict string, char **__restrict end);
long strtol(const char *__restrict string, char **__restrict end, int base);
long long strtoll(const char *__restrict string, char **__restrict end, int base);
unsigned long strtoul(const char *__restrict string, char **__restrict end, int base);
unsigned long long strtoull(const char *__restrict string, char **__restrict end, int base);

// [7.22.2] Pseudo-random sequence generation functions

int rand(void);
int rand_r(unsigned *);
void srand(unsigned int);

// [7.22.3] Memory management functions

void *aligned_alloc(size_t alignment, size_t size);
void *calloc(size_t count, size_t size);
void free(void *pointer);
void *malloc(size_t size);
void *realloc(void *pointer, size_t size);

int posix_memalign(void **, size_t, size_t);

// [7.22.4] Communication with the environment

__attribute__ (( noreturn )) void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
__attribute__ (( noreturn )) void exit(int status);
__attribute__ (( noreturn )) void _Exit(int status);
char *getenv(const char *name);
__attribute__ (( noreturn )) void quick_exit(int status);
int system(const char *string);

// GLIBC extension.
char *mktemp(char *);

// [7.22.5] Searching and sorting utilities

void *bsearch(const void *key, const void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *));
void qsort(void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *));
void qsort_r(void *base, size_t nmemb, size_t size,
		int (*compar)(const void *, const void *, void *),
		void *arg);

// [7.22.6] Integer arithmetic functions

int abs(int number);
long labs(long number);
long long llabs(long long number);

div_t div(int number, int denom);
ldiv_t ldiv(long number, long denom);
lldiv_t lldiv(long long number, long long denom);

// [7.22.7] Multibyte character conversion functions

int mblen(const char *, size_t);
int mbtowc(wchar_t *__restrict wc, const char *__restrict mb_chr, size_t max_size);
int wctomb(char *mb_chr, wchar_t wc);

// [7.22.8] Multibyte string conversion functions

size_t mbstowcs(wchar_t *__restrict wc_string, const char *__restrict mb_string, size_t max_size);
size_t wcstombs(char *mb_string, const wchar_t *__restrict wc_string, size_t max_size);

#ifdef __cplusplus
}
#endif

#ifdef __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_stdlib.h>
#endif

#endif // _STDLIB_H

