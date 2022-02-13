#ifndef _WCHAR_H
#define _WCHAR_H

// This mechanism provides __gnu_va_list which is equivalent to va_list
// We have to do this because wchar.h is not supposed to define va_list
#define __need___va_list
#include <stdarg.h>

#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>
#include <bits/wchar.h>
#include <bits/wint_t.h>
#include <bits/mbstate.h>

#define WEOF 0xffffffffU

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __mlibc_file_base FILE;

typedef struct __mlibc_mbstate mbstate_t;

// MISSING: struct tm

// [7.28.2] Wide formatted I/O functions

int fwprintf(FILE *__restrict, const wchar_t *__restrict, ...);
int fwscanf(FILE *__restrict, const wchar_t *__restrict, ...);
int vfwprintf(FILE *__restrict, const wchar_t *__restrict, __gnuc_va_list);
int vfwscanf(FILE *__restrict, const wchar_t *__restrict, __gnuc_va_list);

int swprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...);
int swscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, ...);
int vswprintf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __gnuc_va_list);
int vswscanf(wchar_t *__restrict, size_t, const wchar_t *__restrict, __gnuc_va_list);

int wprintf(const wchar_t *__restrict, ...);
int wscanf(const wchar_t *__restrict, ...);
int vwprintf(const wchar_t *__restrict, __gnuc_va_list);
int vwscanf(const wchar_t *__restrict, __gnuc_va_list);

// [7.28.3] Wide character I/O functions

wint_t fgetwc(FILE *);
wchar_t *fgetws(wchar_t *__restrict, int, FILE *__restrict);
wint_t fputwc(wchar_t, FILE *);
int fputws(const wchar_t *__restrict, FILE *__restrict);
int fwide(FILE *, int);
wint_t getwc(FILE *);
wint_t getwchar(void);
wint_t putwc(wchar_t, FILE *);
wint_t putwchar(wchar_t);
wint_t ungetwc(wint_t, FILE *);

// [7.28.4] Wide string functions

double wcstod(const wchar_t *__restrict, wchar_t **__restrict);
float wcstof(const wchar_t *__restrict, wchar_t **__restrict);
long double wcstold(const wchar_t *__restrict, wchar_t **__restrict);

long wcstol(const wchar_t *__restrict, wchar_t **__restrict, int);
long long wcstoll(const wchar_t *__restrict, wchar_t **__restrict, int);
unsigned long wcstoul(const wchar_t *__restrict, wchar_t **__restrict, int);
unsigned long long wcstoull(const wchar_t *__restrict, wchar_t **__restrict, int);

wchar_t *wcscpy(wchar_t *__restrict, const wchar_t *__restrict);
wchar_t *wcsncpy(wchar_t *__restrict, const wchar_t *__restrict, size_t);
wchar_t *wmemcpy(wchar_t *__restrict, const wchar_t *__restrict, size_t);
wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t);

wchar_t *wcscat(wchar_t *__restrict, const wchar_t *__restrict);
wchar_t *wcsncat(wchar_t *__restrict, const wchar_t *__restrict, size_t);

int wcscmp(const wchar_t *, const wchar_t *);
int wcscoll(const wchar_t *, const wchar_t *);
int wcsncmp(const wchar_t *, const wchar_t *, size_t);
int wcsxfrm(wchar_t *__restrict, const wchar_t *__restrict, size_t);
int wmemcmp(const wchar_t *, const wchar_t *, size_t);

wchar_t *wcschr(const wchar_t *, wchar_t);
size_t wcscspn(const wchar_t *, const wchar_t *);
wchar_t *wcspbrk(const wchar_t *, const wchar_t *);
wchar_t *wcsrchr(const wchar_t *, wchar_t);
size_t wcsspn(const wchar_t *, const wchar_t *);
wchar_t *wcsstr(const wchar_t *, const wchar_t *);
wchar_t *wcstok(wchar_t *__restrict, const wchar_t *__restrict, wchar_t **__restrict);
wchar_t *wmemchr(const wchar_t *, wchar_t, size_t);

size_t wcslen(const wchar_t *);
wchar_t *wmemset(wchar_t *, wchar_t, size_t);

// [7.28.5] Wide date/time functions

/* POSIX says:
 * The tag tm is declared as naming an incomplete structure type, the contents of which are
 * described in the header <time.h>. */
struct tm;
size_t wcsftime(wchar_t *__restrict, size_t, const wchar_t *__restrict,
		const struct tm *__restrict);

// [7.28.6] Wide conversion functions

wint_t btowc(int c);
int wctob(wint_t);

int mbsinit(const mbstate_t *);
size_t mbrlen(const char *__restrict, size_t, mbstate_t *__restrict);
size_t mbrtowc(wchar_t *__restrict, const char *__restrict, size_t, mbstate_t *__restrict);
size_t wcrtomb(char *__restrict, wchar_t, mbstate_t *__restrict);
size_t mbsrtowcs(wchar_t *__restrict, const char **__restrict, size_t, mbstate_t *__restrict);
size_t mbsnrtowcs(wchar_t *__restrict, const char **__restrict, size_t, size_t, mbstate_t *__restrict);
size_t wcsrtombs(char *__restrict, const wchar_t **__restrict, size_t, mbstate_t *__restrict);
size_t wcsnrtombs(char *__restrict, const wchar_t **__restrict, size_t, size_t, mbstate_t *__restrict);

// POSIX extensions
int wcwidth(wchar_t wc);
int wcswidth(const wchar_t *, size_t);
wchar_t *wcsdup(const wchar_t *s);
int wcsncasecmp(const wchar_t*, const wchar_t*, size_t);

#ifdef __cplusplus
}
#endif

#endif // _WCHAR_H
