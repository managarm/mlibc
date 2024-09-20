#ifndef _GLIBC_MALLOC_H
#define _GLIBC_MALLOC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/size_t.h>

size_t malloc_usable_size(void *__ptr);

#ifdef __cplusplus
}
#endif

#endif /* _GLIBC_MALLOC_H */

