#include <errno.h>
#include <sys/uio.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

ssize_t process_vm_readv(pid_t pid,
    const struct iovec *local_iov, unsigned long liovcnt,
    const struct iovec *remote_iov, unsigned long riovcnt,
    unsigned long flags) {
    MLIBC_CHECK_OR_ENOSYS(mlibc::sys_process_vm_readv, -1);
    ssize_t bytes_read;
    if(int e = mlibc::sys_process_vm_readv(pid, local_iov, liovcnt,
            remote_iov, riovcnt, flags, &bytes_read); e) {
        errno = e;
        return -1;
    }
    return bytes_read;
}

ssize_t process_vm_writev(pid_t pid,
    const struct iovec *local_iov, unsigned long liovcnt,
    const struct iovec *remote_iov, unsigned long riovcnt,
    unsigned long flags) {
    MLIBC_CHECK_OR_ENOSYS(mlibc::sys_process_vm_writev, -1);
    ssize_t bytes_written;
    if(int e = mlibc::sys_process_vm_writev(pid, local_iov, liovcnt,
            remote_iov, riovcnt, flags, &bytes_written); e) {
        errno = e;
        return -1;
    }
    return bytes_written;
}
