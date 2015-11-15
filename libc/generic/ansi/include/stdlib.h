
#ifndef _STDLIB_H
#define _STDLIB_H

#ifdef __cplusplus
extern "C" {
#endif

// [7.22] General utilities

#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

typedef struct {
	int quot, rem;
} div_t;

typedef struct {
	long quot, rem;
} ldiv_t;

typedef struct {
	long long quot, rem;
} lldiv_t;

// NULL is defined in multiple headers
#ifndef NULL
#define NULL 0
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

// MISSING: RAND_MAX
// MISSING: MB_CUR_MAX

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
void srand(unsigned int seed);

// [7.22.3] Memory management functions

void *aligned_alloc(size_t alignment, size_t size);
void *calloc(size_t count, size_t size);
void free(void *pointer);
void *malloc(size_t size);
void *realloc(void *pointer, size_t size);

// [7.22.4] Communication with the environment

__attribute__ (( noreturn )) void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*func)(void));
__attribute__ (( noreturn )) void exit(int status);
__attribute__ (( noreturn )) void _Exit(int status);
char *getenv(const char *name);
__attribute__ (( noreturn )) void quick_exit(int status);
int system(const char *string);

// [7.22.5] Searching and sorting utilities

void *bsearch(const void *key, const void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *));
void qsort(void *base, size_t count, size_t size,
		int (*compare)(const void *, const void *));

// [7.22.6] Integer arithmetic functions

int abs(int number);
long labs(long number);
long long llabs(long long number);

div_t div(int number, int denom);
ldiv_t div(long number, long denom);
lldiv_t div(long long number, long long denom);

// [7.22.7] Multibyte character conversion functions

int mblen(const char *mb_chr, size_t max_size);
int mbtowc(wchar_t *__restrict wc, const char *__restrict mb_chr, size_t max_size);
int wctomb(char *mb_chr, wchar_t wc);

// [7.22.8] Multibyte string conversion functions

int mbstowcs(wchar_t *__restrict wc_string, const char *__restrict mb_string, size_t max_size);
int wcstombs(char *mb_string, const wchar_t *__restrict wc_string, size_t max_size);

#ifdef __cplusplus
}
#endif

#endif // _STDLIB_H

