
#ifndef MLIBC_POSIX_STDIO_H
#define MLIBC_POSIX_STDIO_H

#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>
#include <bits/file.h>

/* MISSING: var_list */

#ifdef __cplusplus
extern "C" {
#endif

#define P_tmpdir "/tmp"

#ifndef __MLIBC_ABI_ONLY

int fileno(FILE *__file);
FILE *fdopen(int __fd, const char *__mode);

FILE *fmemopen(void *__restrict __buf, size_t __size, const char *__restrict __mode);
int pclose(FILE *__file);
FILE *popen(const char *__command, const char *__type);
FILE *open_memstream(char **__buf, size_t *__sizeloc);

int fseeko(FILE *__stream, off_t __offset, int __whence);
int fseeko64(FILE *__stream, off64_t __offset, int __whence);
off_t ftello(FILE *__stream);
off64_t ftello64(FILE *__stream);

__attribute__((format(__printf__, 2, 3))) int dprintf(int __fd, const char *__format, ...);
__attribute__((format(__printf__, 2, 0)))
int vdprintf(int __fd, const char *__format, __builtin_va_list __args);

char *fgetln(FILE *__stream, size_t *__size);

char *tempnam(const char *__dir, const char *__pfx);

#endif /* !__MLIBC_ABI_ONLY */

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

#ifndef __MLIBC_ABI_ONLY

#if defined(_GNU_SOURCE)

FILE *fopencookie(void *__restrict __cookie, const char *__restrict __mode, cookie_io_functions_t __io_funcs);

#endif /* defined(_GNU_SOURCE) */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

/* MISSING: various functions and macros */

#endif /* MLIBC_POSIX_STDIO_H */


