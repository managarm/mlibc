#ifndef _ABIBITS_FD_SET_H
#define _ABIBITS_FD_SET_H

#include <bits/types.h>

#define FD_SETSIZE 1024

typedef __mlibc_uint8 __fd_mask;
#define __NFDBITS (8 * (int) sizeof (__fd_mask))

typedef struct __attribute__((__aligned__(__alignof__(long)))) {
	__fd_mask fds_bits[FD_SETSIZE / __NFDBITS];
} fd_set;

#endif /* _ABIBITS_FD_SET_H */
