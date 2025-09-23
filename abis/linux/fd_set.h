#ifndef _ABIBITS_FD_SET_H
#define _ABIBITS_FD_SET_H

#include <bits/types.h>

#define FD_SETSIZE 1024

typedef struct __attribute__((__aligned__(__alignof__(long)))) {
	__mlibc_uint8 fds_bits[128];
} fd_set;

#endif /* _ABIBITS_FD_SET_H */
