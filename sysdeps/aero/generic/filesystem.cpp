#include "mlibc/fsfd_target.hpp"
#include <aero/syscall.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace mlibc {
int sys_write(int fd, const void *buffer, size_t count, ssize_t *written) {
    auto result = syscall(SYS_WRITE, fd, buffer, count);

    if (result < 0) {
        return -result;
    }

    *written = result;
    return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    auto result = syscall(SYS_READ, fd, buf, count);

    if (result < 0) {
        *bytes_read = 0;
        return -result;
    }

    *bytes_read = result;
    return 0;
}

int sys_pwrite(int fd, const void *buffer, size_t count, off_t off, ssize_t *written) UNIMPLEMENTED("sys_pwrite")
int sys_pread(int fd, void *buf, size_t count, off_t off, ssize_t *bytes_read) UNIMPLEMENTED("sys_pread")

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    auto result = syscall(SYS_SEEK, fd, offset, whence);

    if (result < 0) {
        return -result;
	}

    *new_offset = result;
    return 0;
}

int sys_open(const char *filename, int flags, mode_t mode, int *fd) {
    auto result = syscall(SYS_OPEN, 0, filename, strlen(filename), flags);

    if (result < 0) {
        return -result;
    }

    *fd = result;
    return 0;
}

int sys_close(int fd) {
    auto result = syscall(SYS_CLOSE, fd);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_access(const char *filename, int mode) {
    auto result =
        syscall(SYS_ACCESS, AT_FDCWD, filename, strlen(filename), mode, 0);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
             struct stat *statbuf) {
    auto result = 0;

    switch (fsfdt) {
        case fsfd_target::path: {
            result = syscall(SYS_STAT, path, strlen(path), statbuf);
            break;
        }

        case fsfd_target::fd: {
            result = syscall(SYS_FSTAT, fd, statbuf);
            break;
        }

        default: {
			mlibc::infoLogger() << "mlibc warning: sys_stat: unsupported fsfd target" << frg::endlog;
			return EINVAL;
        }
    }

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    auto sys_res = syscall(SYS_IOCTL, fd, request, arg);

    if (sys_res < 0) {
        return -sys_res;
    }

    *result = sys_res;
    return 0;
}

int sys_isatty(int fd) {
    // NOTE: The easiest way to check if a file descriptor is a TTY is to
    // do an ioctl of TIOCGWINSZ on it and see if it succeeds :^)
    struct winsize ws;
    int result;

    if (!sys_ioctl(fd, TIOCGWINSZ, &ws, &result)) {
        return 0;
    }

    return ENOTTY;
}

int sys_tcgetattr(int fd, struct termios *attr) {
    int result;

    if (int e = sys_ioctl(fd, TCGETS, (void *)attr, &result); e)
        return e;

    return 0;
}

int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
    if (optional_action)
        mlibc::infoLogger()
            << "mlibc: warning: sys_tcsetattr ignores optional_action"
            << frg::endlog;

    int result;

    if (int e = sys_ioctl(fd, TCSETSF, (void *)attr, &result); e)
        return e;

    return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    mlibc::infoLogger() << "sys_poll() is not implemented" << frg::endlog;
    return 0;
}

int sys_mkdir(const char *path, mode_t) {
    auto result = syscall(SYS_MKDIR, path, strlen(path));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_rmdir(const char *path) UNIMPLEMENTED("sys_rmdir")

int sys_link(const char *srcpath, const char *destpath) {
    auto result = syscall(SYS_LINK, srcpath, strlen(srcpath), destpath, strlen(destpath));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_unlinkat(int fd, const char *path, int flags) {
    auto result = syscall(SYS_UNLINK, fd, path, strlen(path), flags);

    if (result < 0) {
        return -result;
	}

    return 0;
}

struct aero_dir_entry {
    size_t inode;
    size_t offset;
    size_t reclen;
    size_t filetyp;
    char name[];
} __attribute__((packed));

int sys_read_entries(int handle, void *buffer, size_t max_size,
                     size_t *bytes_read) {
    auto result = syscall(SYS_GETDENTS, handle, buffer, max_size);

    // Check if we got an error.
    if (result < 0) {
        *bytes_read = 0;
        return -result;
    }

    // Nothing to read.
    if (result == 0) {
        *bytes_read = 0;
        return 0;
    }

    auto entry = (struct aero_dir_entry *)buffer;

    struct dirent dirent = {
        .d_ino = static_cast<ino_t>(entry->inode),
        .d_off = static_cast<off_t>(entry->offset),
        .d_reclen = static_cast<unsigned short>(entry->reclen),
        .d_type = static_cast<unsigned char>(entry->filetyp),
    };

    // The reclen is the size of the dirent struct, plus the size of the name.
    auto name_size = entry->reclen - sizeof(struct aero_dir_entry);
    __ensure(name_size < 255);

    memcpy(&dirent.d_name, entry->name, name_size);
    *bytes_read = entry->reclen;

    memcpy(buffer, &dirent, sizeof(struct dirent));
    return 0;
}

int sys_open_dir(const char *path, int *handle) {
    return sys_open(path, O_DIRECTORY, 0, handle);
}

int sys_rename(const char *path, const char *new_path) UNIMPLEMENTED("sys_rename")

int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) {
    auto result = syscall(SYS_READ_LINK, path, strlen(path), buffer, max_size);

    if (result < 0) {
        return -result;
    }

    *length = result;
    return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
    auto result = syscall(SYS_DUP, fd, flags);

    if (result < 0) {
        return -result;
    }

    *newfd = result;
    return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
    auto result = syscall(SYS_DUP2, fd, newfd, flags);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_fcntl(int fd, int request, va_list args, int *result_value) {
    auto result = syscall(SYS_FCNTL, fd, request, va_arg(args, uint64_t));

    if (result < 0) {
        return -result;
    }

    *result_value = result;
    return 0;
}

int sys_pselect(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
                const struct timespec *timeout, const sigset_t *sigmask,
                int *num_events) {
    mlibc::infoLogger() << "sys_pselect() is not implemented" << frg::endlog;
    return 0;
}

// int sys_chmod(const char *pathname, mode_t mode) UNIMPLEMENTED("sys_chmod")

int sys_pipe(int *fds, int flags) {
    auto result = syscall(SYS_PIPE, fds, flags);

    if (result < 0) {
        return -result;
    }

    return 0;
}

// epoll API syscalls:
int sys_epoll_create(int flags, int *fd) {
    auto result = syscall(SYS_EPOLL_CREATE, flags);

    if (result < 0) {
        return -result;
    }

    *fd = result;
    return 0;
}

int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev) {
    auto result = syscall(SYS_EPOLL_CTL, epfd, mode, fd, ev);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_epoll_pwait(
    int epfd,
    struct epoll_event *ev,
    int n,
    int timeout,
    const sigset_t *sigmask,
    int *raised
) {
    auto result = syscall(SYS_EPOLL_PWAIT, epfd, ev, n, timeout, sigmask);

    if (result < 0) {
        return -result;
    }

    *raised = result;
    return 0;
}

int sys_eventfd_create(unsigned int initval, int flags, int *fd) {
    auto result = syscall(SYS_EVENT_FD, initval, flags);

    if (result < 0) {
        return -result;
    }

    *fd = result;
    return 0;
}
} // namespace mlibc
