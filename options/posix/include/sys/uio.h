#ifndef _SYS_UIO_H
#define _SYS_UIO_H

#include <abi-bits/pid_t.h>
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

// Linux extensions
ssize_t process_vm_readv(pid_t __pid, const struct iovec *__local_iov, unsigned long __liovcnt, const struct iovec *__remote_iov,
						unsigned long __riovcnt, unsigned long __flags);
ssize_t process_vm_writev(pid_t __pid, const struct iovec *__local_iov, unsigned long __liovcnt, const struct iovec *__remote_iov,
						unsigned long __riovcnt, unsigned long __flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SYS_UIO_H */
