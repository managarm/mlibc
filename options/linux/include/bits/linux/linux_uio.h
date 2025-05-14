#ifndef _BITS_LINUX_UIO_H
#define _BITS_LINUX_UIO_H

#include <abi-bits/pid_t.h>
#include <bits/posix/iovec.h>
#include <bits/ssize_t.h>
#include <mlibc-config.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY

ssize_t process_vm_readv(pid_t pid,
		const struct iovec *local_iov, unsigned long liovcnt,
		const struct iovec *remote_iov, unsigned long riovcnt,
		unsigned long flags);
ssize_t process_vm_writev(pid_t pid,
		const struct iovec *local_iov, unsigned long liovcnt,
		const struct iovec *remote_iov, unsigned long riovcnt,
		unsigned long flags);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _BITS_LINUX_UIO_H */
