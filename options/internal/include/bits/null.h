
#ifndef MLIBC_NULL_H
#define MLIBC_NULL_H

#ifdef NULL
#undef NULL
#endif

#ifndef __cplusplus
#  define NULL ((void *)0)
#else
#  define NULL 0
#endif

#endif /* MLIBC_NULL_H */

