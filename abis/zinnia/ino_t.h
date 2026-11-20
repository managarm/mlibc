#ifndef _ABIBITS_INO_T_H
#define _ABIBITS_INO_T_H

#include <bits/types.h>
#include <mlibc-config.h>

typedef __mlibc_uintptr ino_t;

#if defined(_DEFAULT_SOURCE)
typedef ino_t ino64_t;
#endif

#endif /* _ABIBITS_INO_T_H */
