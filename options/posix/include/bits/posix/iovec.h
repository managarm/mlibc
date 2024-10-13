#ifndef MLIBC_IOVEC_H
#define MLIBC_IOVEC_H

#include <bits/types.h>

struct iovec {
	void *iov_base;
	__mlibc_size iov_len;
};

#endif /* MLIBC_IOVEC_H */
