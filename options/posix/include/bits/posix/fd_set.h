#ifndef MLIBC_FD_SET_H
#define MLIBC_FD_SET_H

#include <bits/types.h>

typedef struct {
	union {
		__mlibc_uint8 __mlibc_elems[128];
		/* Some programs require the fds_bits field to be present */
		__mlibc_uint8 fds_bits[128];
	};
} fd_set;

#endif /* MLIBC_FD_SET_H */
