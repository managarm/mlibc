#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/posix/iovec.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>
#include <bits/size_t.h>

#ifdef __cplusplus
extern "C" {
#endif

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

// Non standard extensions, also found on modern BSD's
ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);

#ifdef __cplusplus
}
#endif

#endif // _SYS_UIO_H
