
#ifndef _MLIBC_INTERNAL_NULL_H
#define _MLIBC_INTERNAL_NULL_H

#ifdef NULL
#undef NULL
#endif

#ifndef __cplusplus
#  define NULL ((void *)0)
#else
#  if __cplusplus >= 201103L
#    define NULL nullptr
#  else
#    define NULL __null
#  endif
#endif

#endif /* _MLIBC_INTERNAL_NULL_H */

