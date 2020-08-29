
#ifndef MLIBC_POSIX_STDIO_H
#define MLIBC_POSIX_STDIO_H

#include <bits/off_t.h>
#include <bits/ssize_t.h>

// MISSING: var_list

#ifdef __cplusplus
extern "C" {
#endif

#define P_tmpdir "/tmp"

int fileno(FILE *file);
FILE *fdopen(int fd, const char *mode);

FILE *fmemopen(void *__restrict, size_t, const char *__restrict);
int pclose(FILE *);
FILE *popen(const char*, const char *);
FILE *open_memstream(char **, size_t *);

int fseeko(FILE *stream, off_t offset, int whence);
off_t ftello(FILE *stream);

int dprintf(int fd, const char *format, ...);
int vdprintf(int fd, const char *format, __gnuc_va_list args);

#ifdef __cplusplus
}
#endif

// MISSING: various functions and macros

#endif // MLIBC_POSIX_STDIO_H


