
#ifndef MLIBC_POSIX_STDIO_H
#define MLIBC_POSIX_STDIO_H

#include <mlibc/off_t.h>
#include <mlibc/ssize_t.h>

// MISSING: var_list

#ifdef __cplusplus
extern "C" {
#endif

int fileno(FILE *file);
FILE *fdopen(int fd, const char *mode); 

#ifdef __cplusplus
}
#endif

// MISSING: various functions and macros

#endif // MLIBC_POSIX_STDIO_H


