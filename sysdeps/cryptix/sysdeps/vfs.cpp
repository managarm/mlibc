#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <stdlib.h>

#include <abi-bits/fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cryptix/syscall.h>

namespace mlibc
{
    int sys_sysconf(int num, long* ret)
    {
        switch (num)
        {
            case _SC_OPEN_MAX:
            case _SC_CHILD_MAX:
            case _SC_LINE_MAX:
            case _SC_PHYS_PAGES: *ret = INT32_MAX; break;

            default: return EINVAL;
        }

        return 0;
    }

    int sys_read(int fd, void* buffer, size_t count, ssize_t* bytesRead)
    {
        auto ret = Syscall(SYS_READ, fd, buffer, count);
        if (auto e = syscall_error(ret); e) return e;

        *bytesRead = ssize_t(ret);
        return 0;
    }
    int sys_write(int fd, void const* buffer, unsigned long bytes,
                  long* bytesWritten)
    {

        auto ret = Syscall(SYS_WRITE, fd, buffer, bytes);
        if (auto e = syscall_error(ret); e) return e;

        *bytesWritten = ret;
        return 0;
    }
    int sys_open(const char* path, int flags, mode_t mode, int* fd)
    {
        auto ret = Syscall(SYS_OPENAT, AT_FDCWD, path, flags, mode);
        if (auto e = syscall_error(ret); e) return e;

        *fd = ret;
        return 0;
    }
    int sys_close(int fd)
    {
        auto ret = Syscall(SYS_CLOSE, fd);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    int sys_stat(fsfd_target fsfdt, int fd, const char* path, int flags,
                 struct stat* statbuf)
    {
        int ret = -1;
        switch (fsfdt)
        {
            case fsfd_target::fd:
                ret = Syscall(SYS_FSTATAT, fd, "", flags | AT_EMPTY_PATH,
                              statbuf);
                break;
            case fsfd_target::path:
                ret = Syscall(SYS_FSTATAT, AT_FDCWD, path, flags, statbuf);
                break;
            case fsfd_target::fd_path:
                ret = Syscall(SYS_FSTATAT, fd, path, flags, statbuf);
                break;

            default:
                __ensure(!"sys_stat: invalid fsfdt");
                __builtin_unreachable();
        }
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_seek(int fd, off_t offset, int whence, off_t* newOffset)
    {
        auto ret = Syscall(SYS_LSEEK, fd, offset, whence);
        if (auto e = syscall_error(ret); e) return e;

        *newOffset = off_t(ret);
        return 0;
    }
    int sys_ioctl(int fd, unsigned long request, void* arg, int* result)
    {
        int ret = Syscall(SYS_IOCTL, fd, request, arg);
        if (auto e = syscall_error(ret); e) return e;

        *result = ret;
        return 0;
    }

    int sys_access(const char* path, int mode)
    {
        int ret = Syscall(SYS_ACCESS, path, mode);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_pipe(int* fds, int flags)
    {
        int ret = Syscall(SYS_PIPE, fds);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_dup(int fd, int flags, int* newfd)
    {
        // TODO(v1tr10l7): implement flags;
        (void)flags;
        auto ret = Syscall(SYS_DUP, fd);
        if (auto e = syscall_error(ret); e) return e;

        *newfd = ret;
        return 0;
    }
    int sys_dup2(int fd, int flags, int newfd)
    {
        // TODO(v1tr10l7): implement flags;
        (void)flags;
        auto ret = Syscall(SYS_DUP2, fd, newfd);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    int sys_uname(struct utsname* out)
    {
        auto ret = Syscall(SYS_UNAME, out);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_fcntl(int fd, int request, va_list args, int* result)
    {
        auto ret = Syscall(SYS_FCNTL, fd, request, args);
        if (auto e = syscall_error(ret); e) return e;

        *result = ret;
        return 0;
    }

    int sys_getcwd(char* buffer, size_t size)
    {
        auto ret = Syscall(SYS_GETCWD, buffer, size);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_chdir(const char* path)
    {
        auto ret = Syscall(SYS_CHDIR, path);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_fchdir(int fd)
    {
        auto ret = Syscall(SYS_FCHDIR, fd);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_mkdir(const char* path, mode_t mode)
    {
        auto ret = Syscall(SYS_MKDIR, path, mode);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_mount(const char* source, const char* target, const char* fs_name,
                  unsigned long flags, const void* data)
    {
        auto ret = Syscall(SYS_MOUNT, source, target, fs_name, flags, data);
        if (auto e = syscall_error(ret); e) return e;

        return ret;
    }

    int sys_read_entries(int fdnum, void* buffer, size_t max_size,
                         size_t* bytes_read)
    {
        auto ret = Syscall(SYS_GETDENTS64, fdnum, buffer, max_size);
        if (auto e = syscall_error(ret); e) return e;

        *bytes_read = ret;
        return 0;
    }
    int sys_openat(int dirfd, const char* path, int flags, mode_t mode, int* fd)
    {
        int ret = Syscall(SYS_OPENAT, dirfd, path, flags, mode);
        if (auto e = syscall_error(ret); e) return e;

        *fd = ret;
        return 0;
    }
    int sys_open_dir(const char* path, int* fd)
    {
        return sys_openat(AT_FDCWD, path, O_DIRECTORY, 0, fd);
    }

    int sys_fchmodat(int fd, const char* pathname, mode_t mode, int flags)
    {
        auto ret = Syscall(SYS_FCHMODAT, fd, pathname, mode, flags);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    int sys_isatty(int fd)
    {
        winsize ws;
        int     ret;
        if (!sys_ioctl(fd, TIOCGWINSZ, &ws, &ret)) return 0;

        return ENOTTY;
    }
    int sys_ttyname(int fd, char* buf, size_t size)
    {
        buf[0] = 'T';
        buf[1] = 0;
        return 0;
    }

#ifndef TCGETS
    constexpr size_t TCGETS  = 0x5401;
    constexpr size_t TCSETS  = 0x5402;
    constexpr size_t TCSETSW = 0x5403;
    constexpr size_t TCSETSF = 0x5404;
#endif

    int sys_tcgetattr(int fd, struct termios* attr)
    {
        int ret;
        if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) return r;

        return 0;
    }

    int sys_tcsetattr(int fd, int optional_action, const struct termios* attr)
    {
        int ret;
        switch (optional_action)
        {
            case TCSANOW: optional_action = TCSETS; break;
            case TCSADRAIN: optional_action = TCSETSW; break;
            case TCSAFLUSH: optional_action = TCSETSF; break;
            default: __ensure(!"Unsupported tcsetattr");
        }

        if (int r = sys_ioctl(fd, optional_action, (void*)attr, &ret) != 0)
            return r;

        return 0;
    }
    STUB_RET(int sys_pselect(int nfds, fd_set* readfds, fd_set* writefds,
                             fd_set* exceptfds, const struct timespec* timeout,
                             const sigset_t* sigmask, int* num_events));
} // namespace mlibc
