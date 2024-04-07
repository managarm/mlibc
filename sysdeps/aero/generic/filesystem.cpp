#include "mlibc/fsfd_target.hpp"
#include <aero/syscall.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>

#include <unistd.h>

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

int sys_fsync(int) {
    mlibc::infoLogger() << "\e[35mmlibc: fsync is a stub\e[39m" << frg::endlog;
    return 0;
}

int sys_fdatasync(int) {
    mlibc::infoLogger() << "\e[35mmlibc: fdatasync() is a no-op\e[39m" << frg::endlog;
    return 0;
}

// clang-format off
int sys_pwrite(int fd, const void *buffer, size_t count, off_t off,
               ssize_t *written) UNIMPLEMENTED("sys_pwrite") 

// clang-format off
int sys_pread(int fd, void *buf, size_t count,
                off_t off, ssize_t *bytes_read) UNIMPLEMENTED("sys_pread")

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

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
    auto ret = syscall(SYS_ACCESS, dirfd, pathname, strlen(pathname), mode, flags);
    if(int e = sc_error(ret); e)
        return e;
    return 0;
}

int sys_access(const char *filename, int mode) {
    return sys_faccessat(AT_FDCWD, filename, mode, 0);
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
             struct stat *statbuf) {
    switch (fsfdt) {
    case fsfd_target::path:
        fd = AT_FDCWD;
        break;

    case fsfd_target::fd:
        flags |= AT_EMPTY_PATH;
    
    case fsfd_target::fd_path:
        break;

    default:
        __ensure(!"Invalid fsfd_target");
        __builtin_unreachable();
    }

    auto ret = syscall(SYS_FSTAT, fd, path, strlen(path), flags, statbuf);
    if(int e = sc_error(ret); e)
        return e;
    return 0;
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    auto sys_res = syscall(SYS_IOCTL, fd, request, arg);

    if (sys_res < 0) {
        return -sys_res;
    }

    if (result)
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
    int req;

    switch (optional_action) {
		case TCSANOW: req = TCSETS; break;
		case TCSADRAIN: req = TCSETSW; break;
		case TCSAFLUSH: req = TCSETSF; break;
		default: return EINVAL;
	}

    if (int e = sys_ioctl(fd, req, (void *)attr, NULL); e)
        return e;

    return 0;
}

int sys_mkdir(const char *path, mode_t) {
    auto result = syscall(SYS_MKDIR, path, strlen(path));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_link(const char *srcpath, const char *destpath) {
    auto result =
        syscall(SYS_LINK, srcpath, strlen(srcpath), destpath, strlen(destpath));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_rmdir(const char *path) {
    return sys_unlinkat(AT_FDCWD, path, AT_REMOVEDIR);
}

int sys_unlinkat(int fd, const char *path, int flags) {
    auto ret = syscall(SYS_UNLINK, fd, path, strlen(path), flags);
    if (int e = sc_error(ret); e)
        return e;
    return 0;
}

int sys_symlink(const char *target_path, const char *link_path) {
    return sys_symlinkat(target_path, AT_FDCWD, link_path);
}

int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
    auto ret = syscall(SYS_SYMLINK_AT, dirfd, target_path, strlen(target_path), link_path, strlen(link_path));
    if (int e = sc_error(ret); e)
        return e;
    return 0;
}

struct aero_dir_entry {
    size_t inode;
    size_t offset;
    size_t reclen;
    size_t filetyp;
    char name[];
} __attribute__((__packed__));

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

int sys_rename(const char *path, const char *new_path) {
    auto result =
        syscall(SYS_RENAME, path, strlen(path), new_path, strlen(new_path));

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_readlink(const char *path, void *buffer, size_t max_size,
                 ssize_t *length) {
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

int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout,
                    const sigset_t *sigmask, int *raised) {
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

int sys_ppoll(struct pollfd *fds, int nfds, const struct timespec *timeout,
              const sigset_t *sigmask, int *num_events) {
    auto result = syscall(SYS_POLL, fds, nfds, timeout, sigmask);

    if (result < 0) {
        return -result;
    }

    *num_events = result;
    return 0;
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    struct timespec ts;
    ts.tv_sec = timeout / 1000;
    ts.tv_nsec = (timeout % 1000) * 1000000;

    return sys_ppoll(fds, count, &ts, NULL, num_events);
}

#ifndef MLIBC_BUILDING_RTLD
#include <stdio.h>
int sys_ptsname(int fd, char *buffer, size_t length) {
    int index;
    if (int e = sys_ioctl(fd, TIOCGPTN, &index, NULL); e)
        return e;
    if ((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
        return ERANGE;
    }
    return 0;
}

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set,
                fd_set *except_set, const struct timespec *timeout,
                const sigset_t *sigmask, int *num_events) {
    int fd = epoll_create1(0);
    if (fd == -1)
        return -1;

    for (int k = 0; k < FD_SETSIZE; k++) {
        struct epoll_event ev;
        memset(&ev, 0, sizeof(struct epoll_event));

        if (read_set && FD_ISSET(k, read_set))
            ev.events |= EPOLLIN;
        if (write_set && FD_ISSET(k, write_set))
            ev.events |= EPOLLOUT;
        if (except_set && FD_ISSET(k, except_set))
            ev.events |= EPOLLPRI;

        if (!ev.events)
            continue;

        ev.data.u32 = k;
        if (epoll_ctl(fd, EPOLL_CTL_ADD, k, &ev))
            return -1;
    }

    struct epoll_event evnts[16];
    int n = epoll_pwait(
        fd, evnts, 16,
        timeout ? (timeout->tv_sec * 1000 + timeout->tv_nsec / 100) : -1,
        sigmask);

    if (n == -1)
        return -1;

    fd_set res_read_set;
    fd_set res_write_set;
    fd_set res_except_set;
    FD_ZERO(&res_read_set);
    FD_ZERO(&res_write_set);
    FD_ZERO(&res_except_set);

    int m = 0;

    for (int i = 0; i < n; i++) {
        int k = evnts[i].data.u32;

        if (read_set && FD_ISSET(k, read_set) &&
            evnts[i].events & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
            FD_SET(k, &res_read_set);
            m++;
        }

        if (write_set && FD_ISSET(k, write_set) &&
            evnts[i].events & (EPOLLOUT | EPOLLERR | EPOLLHUP)) {
            FD_SET(k, &res_write_set);
            m++;
        }

        if (except_set && FD_ISSET(k, except_set) &&
            evnts[i].events & EPOLLPRI) {
            FD_SET(k, &res_except_set);
            m++;
        }
    }

    if (close(fd))
        __ensure("mlibc::pselect: close() failed on epoll file");

    if (read_set)
        memcpy(read_set, &res_read_set, sizeof(fd_set));

    if (write_set)
        memcpy(write_set, &res_write_set, sizeof(fd_set));

    if (except_set)
        memcpy(except_set, &res_except_set, sizeof(fd_set));

    *num_events = m;
    return 0;
}
#endif // #ifndef MLIBC_BUILDING_RTLD
} // namespace mlibc
