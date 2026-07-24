#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

#include <piggy/syscall.h>

#include <sys/ioctl.h>

#include <stdlib.h>
#include <string.h>

namespace mlibc {
    int Sysdeps<OpenDir>::operator()(const char* path, int* handle) {
        return sysdep<Open>(path, O_DIRECTORY, 0, handle);
    }

    int Sysdeps<Mkdirat>::operator()(int dirfd, const char* path, mode_t mode) {
        (void) mode;

        long ret = syscall2(SYS_MKDIR, dirfd, (long) path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Mkdir>::operator()(const char* path, mode_t mode) {
        return sysdep<Mkdirat>(AT_FDCWD, path, mode);
    }

    int Sysdeps<Renameat>::operator()(int olddirfd, const char* old_path, int newdirfd, const char* new_path) {
        long ret = syscall4(SYS_RENAME, olddirfd, (long) old_path, newdirfd, (long) new_path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Rename>::operator()(const char *old_path, const char *new_path) {
        return sysdep<Renameat>(AT_FDCWD, old_path, AT_FDCWD, new_path);
    }

    int Sysdeps<Linkat>::operator()(int olddirfd, const char* old_path, int newdirfd, const char* new_path, int flags) {
        (void) flags;

        long ret = syscall4(SYS_LINK, olddirfd, (long) old_path, newdirfd, (long) new_path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Link>::operator()(const char* old_path, const char* new_path) {
        return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
    }

    int Sysdeps<Symlinkat>::operator()(const char* target_path, int dirfd, const char* link_path) {
        long ret = syscall3(SYS_SYMLINK, dirfd, (long) link_path, (long) target_path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Symlink>::operator()(const char* target_path, const char* link_path) {
        return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
    }

    int Sysdeps<Readlinkat>::operator()(int dirfd, const char* path, void* buf, size_t max_size, ssize_t* length) {
        long ret = syscall4(SYS_READLINK, dirfd, (long) path, (long) buf, (long) max_size);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Readlink>::operator()(const char* path, void* buf, size_t max_size, ssize_t* length) {
        return sysdep<Readlinkat>(AT_FDCWD, path, buf, max_size, length);
    }

    int Sysdeps<Unlinkat>::operator()(int fd, const char* path, int flags) {
        (void) flags;

        long ret = syscall2(SYS_UNLINK, fd, (long) path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Rmdir>::operator()(const char* path) {
        return sysdep<Unlinkat>(AT_FDCWD, path, 0);
    }

    int Sysdeps<Pread>::operator()(int fd, void* buf, size_t count, off_t offset, ssize_t* bytes_read) {
        long ret = syscall4(SYS_PREAD, fd, (long) buf, count, offset);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

    int Sysdeps<Pwrite>::operator()(int fd, const void* buf, size_t count, off_t offset, ssize_t* bytes_written) {
        long ret = syscall4(SYS_PWRITE, fd, (long) buf, count, offset);
        if (ret < 0) {
            return -ret;
        }

        *bytes_written = ret;
        return 0;
    }

    int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void* arg, int *result) {
        long ret = syscall3(SYS_IOCTL, fd, request, (long) arg);
        if (ret < 0) {
            return -ret;
        }

        *result = ret;
        return 0;
    }

    int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
        long ret = syscall2(SYS_TRUNCATE, fd, size);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Ppoll>::operator()(struct pollfd* fds, nfds_t count, const struct timespec* timeout, const sigset_t* sigmask, int* num_events) {
        (void) sigmask;

        long ret = syscall4(SYS_POLL, (long) fds, count, (long) timeout, (long) sigmask);
        if (ret < 0) {
            return -ret;
        }

        *num_events = (int) ret;
        return 0;
    }

    int Sysdeps<Poll>::operator()(struct pollfd* fds, nfds_t count, int timeout, int* num_events) {
        if (timeout == -1) {
            return sysdep<Ppoll>(fds, count, NULL, NULL, num_events);
        } else {
            struct timespec ts = { .tv_sec = timeout / 1000, .tv_nsec = (timeout % 1000) * 1000000L };
            return sysdep<Ppoll>(fds, count, &ts, NULL, num_events);
        }
    }

#ifndef MLIBC_BUILDING_RTLD
    int Sysdeps<Pselect>::operator()(int num_fds, fd_set* read_set, fd_set* write_set, fd_set* except_set, const struct timespec* timeout, const sigset_t* sigmask, int* num_events) {
        pollfd* fds = (pollfd*) malloc(num_fds * sizeof(pollfd));
        if (fds == NULL) {
            return ENOMEM;
        }

        int actual_count = 0;

        for(int fd = 0; fd < num_fds; ++fd) {
            short events = 0;
            if (read_set && FD_ISSET(fd, read_set)) {
                events |= POLLIN;
            }

            if (write_set && FD_ISSET(fd, write_set)) {
                events |= POLLOUT;
            }

            if (except_set && FD_ISSET(fd, except_set)) {
                events |= POLLPRI;
            }

            if (events) {
                fds[actual_count].fd = fd;
                fds[actual_count].events = events;
                fds[actual_count].revents = 0;
                actual_count++;
            }
        }

        int num;
        int err = sysdep<Ppoll>(fds, actual_count, timeout, sigmask, &num);
        if (err) {
            free(fds);
            return err;
        }

#define READ_SET_POLLSTUFF (POLLIN | POLLHUP | POLLERR)
#define WRITE_SET_POLLSTUFF (POLLOUT | POLLERR)
#define EXCEPT_SET_POLLSTUFF (POLLPRI)

        int return_count = 0;
        for (int fd = 0; fd < actual_count; fd++) {
            int events = fds[fd].events;
            if ((events & POLLIN) && (fds[fd].revents & READ_SET_POLLSTUFF) == 0) {
                FD_CLR(fds[fd].fd, read_set);
                events &= ~POLLIN;
            }

            if ((events & POLLOUT) && (fds[fd].revents & WRITE_SET_POLLSTUFF) == 0) {
                FD_CLR(fds[fd].fd, write_set);
                events &= ~POLLOUT;
            }

            if ((events & POLLPRI) && (fds[fd].revents & EXCEPT_SET_POLLSTUFF) == 0) {
                FD_CLR(fds[fd].fd, except_set);
                events &= ~POLLPRI;
            }

            if (events) {
                return_count++;
            }
        }

        *num_events = return_count;
        free(fds);
        return 0;
    }
#endif /* MLIBC_BUILDING_RTLD */

    int Sysdeps<Fsync>::operator()(int fd) {
        long ret = syscall1(SYS_SYNC, fd);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    void Sysdeps<Sync>::operator()(void) {
        syscall1(SYS_SYNC, -1);
    }

    int Sysdeps<ReadEntries>::operator()(int handle, void* buffer, size_t max_size, size_t* bytes_read) {
        long ret = syscall3(SYS_GETDENTS, handle, (long) buffer, max_size);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

	int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char* path, int flags, struct stat* statbuf) {
        switch (fsfdt) {
            case fsfd_target::fd:
                flags |= AT_EMPTY_PATH;
                path = "";
                break;
            case fsfd_target::fd_path:
                break;
            case fsfd_target::path:
                fd = AT_FDCWD;
                break;
            default:
                mlibc::infoLogger() << "mlibc: stat: Unknown fsfd_target: " << (int) fsfdt << frg::endlog;
                return ENOSYS;
        }

        long ret = syscall4(SYS_STAT, fd, (long) path, (long) statbuf, flags);
        if (ret < 0) {
            return -ret;
        }

        return 0;
	}

    int Sysdeps<Utimensat>::operator()(int dirfd, const char* pathname, const struct timespec times[2], int flags) {
        if (pathname == NULL) {
            flags |= AT_EMPTY_PATH;
        }

        long ret = syscall4(SYS_UTIME, dirfd, (long) pathname, (long) times, flags);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Fchdir>::operator()(int fd) {
        long ret = syscall1(SYS_CHDIR, fd);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Chdir>::operator()(const char* path) {
        int fd = -1;

        int ret = sysdep<OpenDir>(path, &fd);
        if (ret != 0) {
            return ret;
        }

        ret = sysdep<Fchdir>(fd);

        sysdep<Close>(fd);
        return ret;
    }

    int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int* result) {
        long arg = va_arg(args, long);

        long ret = syscall3(SYS_FCNTL, fd, request, arg);
        if (ret < 0) {
            return -ret;
        }

        *result = ret;
        return 0;
    }

    int Sysdeps<Dup>::operator()(int fd, int flags, int* newfd) {
        (void) flags;

        long ret = syscall3(SYS_FCNTL, fd, F_DUPFD, 0);
        if (ret < 0) {
            return -ret;
        }

        *newfd = ret;
        return 0;
    }

    int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
        long ret = syscall3(SYS_DUP, fd, newfd, flags);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Chroot>::operator()(const char* path) {
        long ret = syscall1(SYS_CHROOT, (long) path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Pipe>::operator()(int* fds, int flags) {
        long ret = syscall2(SYS_PIPE, (long) fds, flags);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Isatty>::operator()(int fd) {
        struct winsize ws;
        int ret;

        if (sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, &ret) == 0) {
            return 0;
        }

        return ENOTTY;
    }

    int Sysdeps<Ttyname>::operator()(int fd, char* buf, size_t size) {
        switch (fd) {
            case STDIN_FILENO:
            case STDOUT_FILENO:
            case STDERR_FILENO:
                memcpy(buf, "/dev/tty", size);
                buf[size - 1] = '\0';
                return 0;
            default:
                return ENOTTY;
        }
    }

    int Sysdeps<Tcgetattr>::operator()(int fd, struct termios* attr){
        int ret;
        return sysdep<Ioctl>(fd, TCGETS, (void*) attr, &ret);
    }

    int Sysdeps<Tcsetattr>::operator()(int fd, int act, const struct termios* attr){
        (void) act;

        int ret;
        return sysdep<Ioctl>(fd, TCSETS, (void*) attr, &ret);
    }

    int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize* winsz) {
        int ret;
        return sysdep<Ioctl>(fd, TIOCGWINSZ, (void*) winsz, &ret);
    }

    int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize* winsz) {
        int ret;
        return sysdep<Ioctl>(fd, TIOCSWINSZ, (void*) winsz, &ret);
    }
} // namespace mlibc
