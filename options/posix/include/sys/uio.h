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

ssize_t readv(int __fd, const struct iovec *__iov, int __iovcnt);
ssize_t writev(int __fd, const struct iovec *__iov, int __iovcnt);

/* Non standard extensions, also found on modern BSD's */
ssize_t preadv(int __fd, const struct iovec *__iov, int __iovcnt, off_t __offset);
ssize_t pwritev(int __fd, const struct iovec *__iov, int __iovcnt, off_t __offset);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UIO_H */
