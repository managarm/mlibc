#ifndef _STDIO_EXT_H
#define _STDIO_EXT_H

#include <stddef.h>
#include <stdio.h>

#define FSETLOCKING_INTERNAL 1
#define FSETLOCKING_BYCALLER 2
#define FSETLOCKING_QUERY 3

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

size_t __fbufsize(FILE *__stream);
size_t __fpending(FILE *__stream);
int __flbf(FILE *__stream);
int __freadable(FILE *__stream);
int __fwritable(FILE *__stream);
int __freading(FILE *__stream);
int __fwriting(FILE *__stream);
int __fsetlocking(FILE *__stream, int __type);
void __fpurge(FILE *__stream);

void _flushlbf(void);

/* The following functions are defined by musl. */

size_t __freadahead(FILE *__stream);
const char *__freadptr(FILE *__stream, size_t *__size);
void __freadptrinc(FILE *, size_t);
void __fseterr(FILE *__stream);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _STDIO_EXT_H */
