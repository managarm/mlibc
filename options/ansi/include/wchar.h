#ifndef _WCHAR_H
#define _WCHAR_H

#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>
#include <bits/wchar.h>
#include <bits/wint_t.h>
#include <bits/mbstate.h>
#include <bits/file.h>

#define WEOF 0xffffffffU

#ifdef __cplusplus
extern "C" {
#endif

/* MISSING: struct tm */

#ifndef __MLIBC_ABI_ONLY

/* [7.28.2] Wide formatted I/O functions */

int fwprintf(FILE *__restrict __stream, const wchar_t *__restrict __format, ...);
int fwscanf(FILE *__restrict __stream, const wchar_t *__restrict __format, ...);
int vfwprintf(FILE *__restrict __stream, const wchar_t *__restrict __format, __builtin_va_list __args);
int vfwscanf(FILE *__restrict __stream, const wchar_t *__restrict __format, __builtin_va_list __args);

int swprintf(wchar_t *__restrict __buffer, size_t __max_size, const wchar_t *__restrict __format, ...);
int swscanf(wchar_t *__restrict __buffer, const wchar_t *__restrict __format, ...);
int vswprintf(wchar_t *__restrict __buffer, size_t __max_size, const wchar_t *__restrict __format, __builtin_va_list __args);
int vswscanf(wchar_t *__restrict __buffer, const wchar_t *__restrict __format, __builtin_va_list __args);

int wprintf(const wchar_t *__restrict __format, ...);
int wscanf(const wchar_t *__restrict __format, ...);
int vwprintf(const wchar_t *__restrict __format, __builtin_va_list __args);
int vwscanf(const wchar_t *__restrict __format, __builtin_va_list __args);

/* [7.28.3] Wide character I/O functions */

wint_t fgetwc(FILE *__stream);
wchar_t *fgetws(wchar_t *__restrict __buffer, int __size, FILE *__restrict __stream);
wint_t fputwc(wchar_t __wc, FILE *__stream);
int fputws(const wchar_t *__restrict __buffer, FILE *__restrict __stream);
int fwide(FILE *__stream, int __mode);
wint_t getwc(FILE *__stream);
wint_t getwchar(void);
wint_t putwc(wchar_t __wc, FILE *__stream);
wint_t putwchar(wchar_t __wc);
wint_t ungetwc(wint_t __wc, FILE *__stream);

/* [7.28.4] Wide string functions */

double wcstod(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr);
float wcstof(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr);
long double wcstold(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr);

long wcstol(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr, int __base);
long long wcstoll(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr, int __base);
unsigned long wcstoul(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr, int __base);
unsigned long long wcstoull(const wchar_t *__restrict __nptr, wchar_t **__restrict __endptr, int __base);

wchar_t *wcscpy(wchar_t *__restrict __dest, const wchar_t *__restrict __src);
wchar_t *wcsncpy(wchar_t *__restrict __dest, const wchar_t *__restrict __src, size_t __size);
wchar_t *wmemcpy(wchar_t *__restrict __dest, const wchar_t *__restrict __src, size_t __size);
wchar_t *wmemmove(wchar_t *__dest, const wchar_t *__src, size_t __size);

wchar_t *wcscat(wchar_t *__restrict __dest, const wchar_t *__restrict __src);
wchar_t *wcsncat(wchar_t *__restrict __dest, const wchar_t *__restrict __src, size_t __size);

int wcscmp(const wchar_t *__a, const wchar_t *__b);
int wcscoll(const wchar_t *__a, const wchar_t *__b);
int wcsncmp(const wchar_t *__a, const wchar_t *__b, size_t __size);
int wcsxfrm(wchar_t *__restrict __dest, const wchar_t *__restrict __src, size_t __size);
int wmemcmp(const wchar_t *__a, const wchar_t *__b, size_t __size);

wchar_t *wcschr(const wchar_t *__s, wchar_t __wc);
size_t wcscspn(const wchar_t *__dest, const wchar_t *__wchrs);
wchar_t *wcspbrk(const wchar_t *__s, const wchar_t *__wchrs);
wchar_t *wcsrchr(const wchar_t *__s, wchar_t __wc);
size_t wcsspn(const wchar_t *__s, const wchar_t *__wchrs);
wchar_t *wcsstr(const wchar_t *__s, const wchar_t *__b);
wchar_t *wcstok(wchar_t *__restrict __s, const wchar_t *__restrict __delimiter, wchar_t **__restrict __ptr);
wchar_t *wmemchr(const wchar_t *__s, wchar_t __wc, size_t __size);

size_t wcslen(const wchar_t *__s);
wchar_t *wmemset(wchar_t *__dest, wchar_t __wc, size_t __size);

/* [7.28.5] Wide date/time functions */

/* POSIX says:
 * The tag tm is declared as naming an incomplete structure type, the contents of which are
 * described in the header <time.h>. */
struct tm;
size_t wcsftime(wchar_t *__restrict __buffer, size_t __max_size, const wchar_t *__restrict __format,
		const struct tm *__restrict __time);

/* [7.28.6] Wide conversion functions */

wint_t btowc(int __wc);
int wctob(wint_t __wc);

int mbsinit(const mbstate_t *__state);
size_t mbrlen(const char *__restrict __mbs, size_t __mbs_limit, mbstate_t *__restrict __stp);
size_t mbrtowc(wchar_t *__restrict __wcp, const char *__restrict __mbs, size_t __mbs_limit, mbstate_t *__restrict __stp);
size_t wcrtomb(char *__restrict __mbs, wchar_t __wc, mbstate_t *__restrict __stp);
size_t mbsrtowcs(wchar_t *__restrict __wcs, const char **__restrict __mbs, size_t __mb_limit, mbstate_t *__restrict __stp);
size_t mbsnrtowcs(wchar_t *__restrict __wcs, const char **__restrict __mbs, size_t __mb_limit, size_t __wc_limit,
		mbstate_t *__restrict __stp);
size_t wcsrtombs(char *__restrict __mbs, const wchar_t **__restrict __wcs, size_t __mb_limit, mbstate_t *__restrict __stp);
size_t wcsnrtombs(char *__restrict __mbs, const wchar_t **__restrict __wcs, size_t __mb_limit, size_t __wc_limit,
		mbstate_t *__restrict __stp);

/* POSIX extensions */
int wcwidth(wchar_t __wc);
int wcswidth(const wchar_t *__s, size_t __size);
wchar_t *wcsdup(const wchar_t *__s);
int wcsncasecmp(const wchar_t *__a, const wchar_t *__b, size_t __size);
int wcscasecmp(const wchar_t *__a, const wchar_t *__b);
size_t wcsnlen(const wchar_t *__s, size_t __maxlen);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _WCHAR_H */
