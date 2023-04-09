
#ifndef _MALLOC_H
#define _MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>
#include <mlibc-config.h>

#ifndef __MLIBC_ABI_ONLY

// [7.22.3] Memory management functions
void *calloc(size_t count, size_t size);
void free(void *pointer);
void *malloc(size_t size);
void *realloc(void *pointer, size_t size);
void *memalign(size_t, size_t);

#if __MLIBC_GLIBC_OPTION
#include <bits/glibc/glibc_malloc.h>
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _MALLOC_H

