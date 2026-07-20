#ifndef _MLIBC_INTERNAL_PTHREAD_ATTR_T_H
#define _MLIBC_INTERNAL_PTHREAD_ATTR_T_H

#include <bits/types.h>

typedef struct {
	void *__heap_ptr;
} pthread_attr_t;

#endif /* _MLIBC_INTERNAL_PTHREAD_ATTR_T_H */
