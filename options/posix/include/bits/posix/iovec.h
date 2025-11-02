#ifndef _MLIBC_POSIX_IOVEC_H
#define _MLIBC_POSIX_IOVEC_H

#include <bits/types.h>

struct iovec {
	void *iov_base;
	__mlibc_size iov_len;
};

#endif /* _MLIBC_POSIX_IOVEC_H */
