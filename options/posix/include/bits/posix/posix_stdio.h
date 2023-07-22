
#ifndef MLIBC_POSIX_STDIO_H
#define MLIBC_POSIX_STDIO_H

#include <bits/off_t.h>
#include <bits/size_t.h>
#include <bits/ssize_t.h>

// MISSING: var_list

#ifdef __cplusplus
extern "C" {
#endif

#define P_tmpdir "/tmp"

#ifndef __MLIBC_ABI_ONLY

int fileno(FILE *file);
FILE *fdopen(int fd, const char *mode);

FILE *fmemopen(void *__restrict, size_t, const char *__restrict);
int pclose(FILE *);
FILE *popen(const char*, const char *);
FILE *open_memstream(char **, size_t *);

int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);

int dprintf(int fd, const char *format, ...);
int vdprintf(int fd, const char *format, __builtin_va_list args);

char *fgetln(FILE *, size_t *);

#endif /* !__MLIBC_ABI_ONLY */

#define RENAME_EXCHANGE (1 << 1)

// GNU extensions
typedef ssize_t (cookie_read_function_t)(void *, char *, size_t);
typedef ssize_t (cookie_write_function_t)(void *, const char *, size_t);
typedef int (cookie_seek_function_t)(void *, off_t *, int);
typedef int (cookie_close_function_t)(void *);

typedef struct _IO_cookie_io_functions_t {
	cookie_read_function_t *read;
	cookie_write_function_t *write;
	cookie_seek_function_t *seek;
	cookie_close_function_t *close;
} cookie_io_functions_t;

#ifndef __MLIBC_ABI_ONLY

#if defined(_GNU_SOURCE)

FILE *fopencookie(void *__restrict cookie, const char *__restrict mode, cookie_io_functions_t io_funcs);

#endif // defined(_GNU_SOURCE)

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

// MISSING: various functions and macros

#endif /* MLIBC_POSIX_STDIO_H */


