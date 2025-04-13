#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <aarch64/syscall.h>
#include <string.h>

namespace mlibc
{
    int sys_open(const char *pathname, int flags, mode_t mode, int *fd)
    {
        return sys_openat(AT_FDCWD, pathname, flags, mode, fd);
    }

    int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd)
    {
        long ret = __do_syscall5(OPENAT, dirfd, strlen(path), path, flags, mode);
        if (ret < 0)
        {
            return -ret;
        }
        *fd = ret;
        return 0;
    }

    int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read)
    {
        long ret = __do_syscall4(PREAD, fd, buf, n, off);
        if (ret < 0)
        {
            return -ret;
        }
        *bytes_read = ret;
        return 0;
    }

    int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read)
    {
        return sys_pread(fd, buf, count, 0, bytes_read);
    }

    int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written)
    {
        long ret = __do_syscall4(PWRITE, fd, buf, n, off);
        if (ret < 0)
        {
            return -ret;
        }
        *bytes_written = ret;
        return 0;
    }

    int sys_write(int fd, const void *data, size_t size, ssize_t *bytes_written)
    {
        return sys_pwrite(fd, data, size, 0, bytes_written);
    }

    int sys_seek(int /* fd */, off_t /* offset */, int /* whence */, off_t * /* new_offset */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_close(int fd)
    {
        return __do_syscall1(CLOSE, fd);
    }

    int sys_stat(fsfd_target /* fsfdt */, int /* fd */, const char * /* path */, int /* flags */,
                 struct stat * /* statbuf */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_dup2(int fd, int flags, int newfd)
    {
        long ret = __do_syscall3(DUP3, fd, newfd, flags);
        if (ret < 0)
        {
            return -ret;
        }
        return 0;
    }

    int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window)
    {
        long ret = __do_syscall6(MMAP, hint, size, prot, flags, fd, offset);
        if (ret < 0)
        {
            return -ret;
        }
        *window = (void *)ret;
        return 0;
    }

    int sys_vm_unmap(void *pointer, size_t size)
    {
        long ret = __do_syscall2(MUNMAP, pointer, size);
        if (ret < 0)
        {
            return -ret;
        }
        return 0;
    }
} // namespace mlibc