#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <bits/posix/iovec.h>
#include <bits/ssize_t.h>
#include <bits/off_t.h>
#include <bits/size_t.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UIO_MAXIOV IOV_MAX

#ifndef __MLIBC_ABI_ONLY

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev(int fd, const struct iovec *iov, int iovcnt);

// Non standard extensions, also found on modern BSD's
ssize_t preadv(int fd, const struct iovec *iov, int iovcnt, off_t offset);
ssize_t pwritev(int fd, const struct iovec *iov, int iovcnt, off_t offset);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SYS_UIO_H
