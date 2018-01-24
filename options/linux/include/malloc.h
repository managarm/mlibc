
#ifndef _MALLOC_H
#define _MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

// [7.22.3] Memory management functions
void *calloc(size_t count, size_t size);
void free(void *pointer);
void *malloc(size_t size);
void *realloc(void *pointer, size_t size);
void *memalign(size_t, size_t);

#ifdef __cplusplus
}
#endif

#endif // _MALLOC_H

