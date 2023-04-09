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

size_t __fbufsize(FILE *);
size_t __fpending(FILE *);
int __flbf(FILE *);
int __freadable(FILE *);
int __fwritable(FILE *);
int __freading(FILE *);
int __fwriting(FILE *);
int __fsetlocking(FILE *, int);
void __fpurge(FILE *);

void _flushlbf(void);

// The following functions are defined by musl.

size_t __freadahead(FILE *);
const char *__freadptr(FILE *, size_t *);
void __fseterr(FILE *);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _STDIO_EXT_H
