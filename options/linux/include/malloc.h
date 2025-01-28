
#ifndef _MALLOC_H
#define _MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>
#include <mlibc-config.h>

#ifndef __MLIBC_ABI_ONLY

/* [7.22.3] Memory management functions */
void *calloc(size_t __count, size_t __size);
void free(void *__pointer);
void *malloc(size_t __size);
void *realloc(void *__pointer, size_t __size);
void *memalign(size_t __alignment, size_t __size);

#if __MLIBC_GLIBC_OPTION
#include <bits/glibc/glibc_malloc.h>
#endif

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _MALLOC_H */

