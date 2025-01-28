#ifndef MLIBC_FD_SET_H
#define MLIBC_FD_SET_H

#include <bits/types.h>

typedef struct {
	__mlibc_uint8 fds_bits[128];
} fd_set;

#endif /* MLIBC_FD_SET_H */
