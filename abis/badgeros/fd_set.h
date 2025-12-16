#ifndef _ABIBITS_FD_SET_H
#define _ABIBITS_FD_SET_H

#include <bits/types.h>

#define FD_SETSIZE 256

typedef struct {
	__mlibc_uint8 fds_bits[FD_SETSIZE / 8];
} fd_set;

#endif /* _ABIBITS_FD_SET_H */
