#ifndef MLIBC_IOVEC_H
#define MLIBC_IOVEC_H

struct iovec {
	void *iov_base;
	__SIZE_TYPE__ iov_len;
};

#endif // MLIBC_IOVEC_H
