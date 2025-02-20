#ifndef _STRING_H
#define _STRING_H

#include <mlibc-config.h>
#include <bits/null.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

/* [7.24.2] Copying functions */

void *memcpy(void *__restrict __dest, const void *__restrict __src, size_t __size);
void *memmove(void *__dest, const void *__src, size_t __size);
char *strcpy(char *__restrict __dest, const char *src);
char *strncpy(char *__restrict __dest, const char *__src, size_t __max_size);

/* [7.24.3] Concatenation functions */

char *strcat(char *__restrict __dest, const char *__restrict __src);
char *strncat(char *__restrict __dest, const char *__restrict __src, size_t __max_size);

/* [7.24.4] Comparison functions */

int memcmp(const void *__a, const void *__b, size_t __size);
int strcmp(const char *__a, const char *__b);
int strcoll(const char *__a, const char *__b);
int strncmp(const char *__a, const char *__b, size_t __max_size);
size_t strxfrm(char *__restrict __dest, const char *__restrict __src, size_t __max_size);

/* [7.24.5] Search functions */

void *memchr(const void *__s, int __c, size_t __size);
char *strchr(const char *__s, int __c);
size_t strcspn(const char *__s, const char *__chrs);
char *strpbrk(const char *__s, const char *__chrs);
char *strrchr(const char *__s, int __c);
size_t strspn(const char *__s, const char *__chrs);
char *strstr(const char *__pattern, const char *__s);
char *strtok(char *__restrict __s, const char *__restrict __delimiter);

/* This is a GNU extension. */
char *strchrnul(const char * __s, int __c);

/* [7.24.6] Miscellaneous functions */

void *memset(void *__dest, int __c, size_t __size);
char *strerror(int __errnum);
size_t strlen(const char *__s);

#endif /* !__MLIBC_ABI_ONLY */

#if __MLIBC_POSIX_OPTION && (defined(_BSD_SOURCE) || defined(_GNU_SOURCE))
#include <strings.h>
#endif

#ifndef __MLIBC_ABI_ONLY

/* POSIX extensions. */
#if defined(_GNU_SOURCE)
char *strerror_r(int __errnum, char *__buffer, size_t __size) __asm__("__gnu_strerror_r");
#else
int strerror_r(int __errnum, char *__buffer, size_t __size);
#endif

void *mempcpy(void *__dest, const void *__src, size_t __size);

/* GNU extensions. */
int strverscmp(const char *__l0, const char *__r0);
int ffsl(long __i);
int ffsll(long long __i);
void *memmem(const void *__haystack, size_t __haystacklen, const void *__needle, size_t __needlelen);

/* Handling the basename mess:
 * If <libgen.h> is included *at all*, we use the XPG-defined basename
 * implementation, otherwise, we use the GNU one. Since our ABI previously
 * provided the XPG one under basename, we'll have to diverge from GNU here and
 * provide __mlibc_gnu_basename instead.
 */
#if __MLIBC_GLIBC_OPTION && defined(_GNU_SOURCE) && !defined(basename)
char *__mlibc_gnu_basename_c(const char *__path);

# ifdef __cplusplus
extern "C++" {
static inline const char *__mlibc_gnu_basename(const char *__path) {
	return __mlibc_gnu_basename_c(__path);
}
static inline char *__mlibc_gnu_basename(char *__path) {
	return __mlibc_gnu_basename_c(__path);
}
}
# else
#  define __mlibc_gnu_basename __mlibc_gnu_basename_c
# endif

#define basename __mlibc_gnu_basename
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#if __MLIBC_POSIX_OPTION
#	include <bits/posix/posix_string.h>
#endif

#endif /* _STRING_H */
