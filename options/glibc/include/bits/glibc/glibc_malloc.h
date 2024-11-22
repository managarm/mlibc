#ifndef _GLIBC_MALLOC_H
#define _GLIBC_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>
#include <bits/file.h>

size_t malloc_usable_size(void *__ptr);
int malloc_info(int options, FILE *stream);

#ifdef __cplusplus
}
#endif

#endif /* _GLIBC_MALLOC_H */

