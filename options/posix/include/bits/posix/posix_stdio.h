
#ifndef _MLIBC_POSIX_STDIO_H
#define _MLIBC_POSIX_STDIO_H

#include <mlibc-config.h>

#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/file.h>

/* MISSING: var_list */

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_DEFAULT_SOURCE) || (defined(__MLIBC_XOPEN) && __MLIBC_XOPEN < 800)
#define P_tmpdir "/tmp"

char *tempnam(const char *__dir, const char *__pfx);
#endif /* defined(_DEFAULT_SOURCE) || (defined(__MLIBC_XOPEN) && __MLIBC_XOPEN < 800) */

#define L_ctermid 20

#ifndef __MLIBC_ABI_ONLY

char *ctermid(char *__s);

int fileno(FILE *__file);
FILE *fdopen(int __fd, const char *__mode);

FILE *fmemopen(void *__restrict __buf, size_t __size, const char *__restrict __mode);
int pclose(FILE *__file);
FILE *popen(const char *__command, const char *__type);
FILE *open_memstream(char **__buf, size_t *__sizeloc);

int fseeko(FILE *__stream, off_t __offset, int __whence);
off_t ftello(FILE *__stream);

#if __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE)
int fseeko64(FILE *__stream, off64_t __offset, int __whence);
off64_t ftello64(FILE *__stream);
#endif /* !__MLIBC_LINUX_OPTION */

__attribute__((format(__printf__, 2, 3))) int dprintf(int __fd, const char *__format, ...);
__attribute__((format(__printf__, 2, 0)))
int vdprintf(int __fd, const char *__format, __builtin_va_list __args);

#if defined(_GNU_SOURCE)
char *fgetln(FILE *__stream, size_t *__size);
#endif /* defined(_GNU_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#if defined(_GNU_SOURCE)
#define RENAME_EXCHANGE (1 << 1)

/* GNU extensions */
typedef ssize_t (cookie_read_function_t)(void *__cookie, char *__buffer, size_t __size);
typedef ssize_t (cookie_write_function_t)(void *__cookie, const char *__buffer, size_t __size);
typedef int (cookie_seek_function_t)(void *__cookie, off_t *, int);
typedef int (cookie_close_function_t)(void *__cookie);

typedef struct _IO_cookie_io_functions_t {
	cookie_read_function_t *read;
	cookie_write_function_t *write;
	cookie_seek_function_t *seek;
	cookie_close_function_t *close;
} cookie_io_functions_t;
#endif /* defined(_GNU_SOURCE) */

#ifndef __MLIBC_ABI_ONLY

#if defined(_GNU_SOURCE)

FILE *fopencookie(void *__restrict __cookie, const char *__restrict __mode, cookie_io_functions_t __io_funcs);

#endif /* defined(_GNU_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

/* MISSING: various functions and macros */

#endif /* _MLIBC_POSIX_STDIO_H */


