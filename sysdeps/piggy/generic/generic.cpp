#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

#include <piggy/archctl.h>
#include <piggy/syscall.h>

#include <sys/ioctl.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

namespace mlibc {
    [[noreturn]] void Sysdeps<Exit>::operator()(int status) {
        syscall1(SYS_EXIT, status);
        __builtin_unreachable();
    }

    int Sysdeps<Fork>::operator()(pid_t *pid) {
        long ret = syscall0(SYS_FORK);
        if (ret < 0) {
            return -ret;
        }

        *pid = ret;
        return 0;
    }

    int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
        long ret = syscall3(SYS_EXEC, (long) path, (long) argv, (long) envp);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
        (void) ru;

        long ret = syscall3(SYS_WAIT, pid, (long) status, flags);
        if (ret < 0) {
            return -ret;
        }

        *ret_pid = ret;
        return 0;
    }

    int Sysdeps<Kill>::operator()(pid_t pid, int signal) {
        long ret = syscall2(SYS_KILL, pid, signal);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    pid_t Sysdeps<GetPid>::operator()(void) {
        return syscall0(SYS_GETPID);
    }

    pid_t Sysdeps<GetPpid>::operator()(void) {
        return syscall0(SYS_GETPPID);
    }

    pid_t Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
        long ret = syscall1(SYS_GETPGID, pid);
        if (ret < 0) {
            return -ret;
        }

        *pgid = ret;
        return 0;
    }

    int Sysdeps<SetPgid>::operator()(pid_t pgid, pid_t pid) {
        long ret = syscall2(SYS_SETPGID, pid, pgid);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    pid_t Sysdeps<GetTid>::operator()(void) {
        return syscall0(SYS_GETTID);
    }

    void Sysdeps<Yield>::operator()(void) {
        syscall0(SYS_YIELD);
    }

    int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
        (void) mode;

        long ret = syscall3(SYS_OPEN, dirfd, (long) path, flags);
        if (ret < 0) {
            return -ret;
        }

        *fd = ret;
        return 0;
    }

	int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
        return sysdep<Openat>(AT_FDCWD, pathname, flags, mode, fd);
	};

    int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
        return sysdep<Open>(path, O_DIRECTORY, 0, handle);
    }

    int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
        (void) mode;

        long ret = syscall2(SYS_MKDIR, dirfd, (long) path);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
        return sysdep<Mkdirat>(AT_FDCWD, path, mode);
    }

    int Sysdeps<Renameat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
        long ret = syscall4(SYS_RENAME, olddirfd, (long) old_path, newdirfd, (long) new_path);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Rename>::operator()(const char *old_path, const char *new_path) {
        return sysdep<Renameat>(AT_FDCWD, old_path, AT_FDCWD, new_path);
    }

    int Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
        (void) flags;

        long ret = syscall4(SYS_LINK, olddirfd, (long) old_path, newdirfd, (long) new_path);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
        return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
    }

    int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
        long ret = syscall3(SYS_SYMLINK, dirfd, (long) link_path, (long) target_path);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
        return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
    }

    int Sysdeps<Readlinkat>::operator()(int dirfd, const char *path, void *buf, size_t max_size, ssize_t *length) {
        long ret = syscall4(SYS_READLINK, dirfd, (long) path, (long) buf, (long) max_size);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Readlink>::operator()(const char *path, void *buf, size_t max_size, ssize_t *length) {
        return sysdep<Readlinkat>(AT_FDCWD, path, buf, max_size, length);
    }

    int Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags) {
        (void) flags;

        long ret = syscall2(SYS_UNLINK, fd, (long) path);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Rmdir>::operator()(const char *path) {
        return sysdep<Unlinkat>(AT_FDCWD, path, 0);
    }

    int Sysdeps<Close>::operator()(int fd) {
        long ret = syscall1(SYS_CLOSE, fd);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Read>::operator()(int fd, void *buf, size_t count, ssize_t *bytes_read) {
        long ret = syscall3(SYS_READ, fd, (long) buf, count);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

    int Sysdeps<Write>::operator()(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
        long ret = syscall3(SYS_WRITE, fd, (long) buf, count);
        if (ret < 0) {
            return -ret;
        }

        *bytes_written = ret;
        return 0;
    }

    int Sysdeps<Pread>::operator()(int fd, void *buf, size_t count, off_t offset, ssize_t *bytes_read) {
        long ret = syscall4(SYS_PREAD, fd, (long) buf, count, offset);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

    int Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t count, off_t offset, ssize_t *bytes_written) {
        long ret = syscall4(SYS_PWRITE, fd, (long) buf, count, offset);
        if (ret < 0) {
            return -ret;
        }

        *bytes_written = ret;
        return 0;
    }

    int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
        long ret = syscall3(SYS_IOCTL, fd, request, (long) arg);
        if (ret < 0) {
            return -ret;
        }

        *result = ret;
        return 0;
    }

    int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
        long ret = syscall3(SYS_SEEK, fd, offset, whence);
        if (ret < 0) {
            return -ret;
        }

        *new_offset = ret;
        return 0;
    }

	int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
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

    int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
        long ret = syscall2(SYS_TRUNCATE, fd, size);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Utimensat>::operator()(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
        if (pathname == NULL) {
            flags |= AT_EMPTY_PATH;
        }

        long ret = syscall4(SYS_UTIME, dirfd, (long) pathname, (long) times, flags);
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
    int Sysdeps<Pselect>::operator()(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
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

    int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
        long ret = syscall3(SYS_GETDENTS, handle, (long) buffer, max_size);
        if (ret < 0) {
            return -ret;
        }

        *bytes_read = ret;
        return 0;
    }

    int Sysdeps<Chdir>::operator()(const char *path) {
        int fd = -1;

        int ret = sysdep<OpenDir>(path, &fd);
        if (ret != 0) {
            return ret;
        }

        ret = sysdep<Fchdir>(fd);

        sysdep<Close>(fd);
        return ret;
    }

    int Sysdeps<Fchdir>::operator()(int fd) {
        long ret = syscall1(SYS_CHDIR, fd);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
        long arg = va_arg(args, long);

        long ret = syscall3(SYS_FCNTL, fd, request, arg);
        if (ret < 0) {
            return -ret;
        }

        *result = ret;
        return 0;
    }

    int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
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

    int Sysdeps<Isatty>::operator()(int fd) {
        struct winsize ws;
        int ret;

        if (sysdep<Ioctl>(fd, TIOCGWINSZ, &ws, &ret) == 0) {
            return 0;
        }

        return ENOTTY;
    }

    int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr){
        int res;
        return sysdep<Ioctl>(fd, TCGETS, (void*) attr, &res);
    }

    int Sysdeps<Tcsetattr>::operator()(int fd, int act, const struct termios *attr){
        (void) act;

        int res;
        return sysdep<Ioctl>(fd, TCSETS, (void*) attr, &res);
    }

    int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
        int res;
        return sysdep<Ioctl>(fd, TIOCGWINSZ, (void*) winsz, &res);
    }

    int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
        int res;
        return sysdep<Ioctl>(fd, TIOCSWINSZ, (void*) winsz, &res);
    }

    int Sysdeps<Chroot>::operator()(const char *path) {
        long ret = syscall1(SYS_CHROOT, (long) path);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Pipe>::operator()(int *fds, int flags) {
        long ret = syscall2(SYS_PIPE, (long) fds, flags);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
        struct timespec ts;
        ts.tv_sec = *secs;
        ts.tv_nsec = *nanos;

        long ret = syscall1(SYS_SLEEP, (long) &ts);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
        struct timespec ts;

        long ret = syscall2(SYS_GETCLOCK, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;
        return 0;
    }

    int Sysdeps<ClockGetres>::operator()(int clock, time_t *secs, long *nanos) {
        struct timespec ts;

        long ret = syscall2(SYS_GETCLOCKRES, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }

        *secs = ts.tv_sec;
        *nanos = ts.tv_nsec;
        return 0;
    }

    int Sysdeps<ClockSet>::operator()(int clock, time_t secs, long nanos) {
        struct timespec ts = { .tv_sec = secs, .tv_nsec = nanos };

        long ret = syscall2(SYS_SETCLOCK, clock, (long) &ts);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

#ifndef MLIBC_BUILDING_RTLD
	extern "C" void __mlibc_restorer();

    int Sysdeps<Sigaction>::operator()(int signal, const struct sigaction *__restrict act, struct sigaction *__restrict oldact) {
        if (act != nullptr) {
            struct sigaction modified = *act;
            modified.sa_restorer = __mlibc_restorer;

            long ret = syscall3(SYS_SIGACTION, signal, (long) &modified, (long) oldact);
            if (ret < 0) {
                return -ret;
            }
            return 0;
        }

        long ret = syscall3(SYS_SIGACTION, signal, 0, (long) oldact);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }
#endif

    int Sysdeps<Sigaltstack>::operator()(const stack_t *__restrict ss, stack_t *__restrict oldss) {
        long ret = syscall2(SYS_SIGALTSTACK, (long) ss, (long) oldss);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Sigpending>::operator()(sigset_t *set) {
        long ret = syscall1(SYS_SIGPENDING, (long) set);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
        long ret = syscall3(SYS_SIGPROCMASK, how, (long) set, (long) retrieve);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Sigsuspend>::operator()(const sigset_t *mask) {
        long ret = syscall1(SYS_SIGSUSPEND, (long) mask);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
        long ret = syscall3(SYS_SOCKET, family, type, protocol);
        if (ret < 0) {
            return -ret;
        }

        *fd = ret;
        return 0;
    }

    int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
        long ret = syscall3(SYS_BIND, fd, (long) addr_ptr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
        long ret = syscall3(SYS_CONNECT, fd, (long) addr_ptr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
        (void) flags;

        long ret = syscall5(SYS_RECV, fd, (long) buffer, size, (long) sock_addr, (long) addr_length);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags, const sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
        (void) flags;

        long ret = syscall5(SYS_SEND, fd, (long) buffer, size, (long) sock_addr, addr_length);
        if (ret < 0) {
            return -ret;
        }

        *length = ret;
        return 0;
    }

    int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
        long ret = syscall3(SYS_GETSOCKNAME, fd, (long) addr_ptr, max_addr_length);
        if (ret < 0) {
            return -ret;
        }

        *actual_length = ret;
        return 0;
    }

    int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
        long ret = syscall3(SYS_GETPEERNAME, fd, (long) addr_ptr, max_addr_length);
        if (ret < 0) {
            return -ret;
        }

        *actual_length = ret;
        return 0;
    }

    int Sysdeps<Shutdown>::operator()(int fd, int how) {
        long ret = syscall2(SYS_SHUTDOWN, fd, how);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

#ifndef MLIBC_BUILDING_RTLD
    extern "C" void __mlibc_thread_entry();

    int Sysdeps<Clone>::operator()(void *tcb, pid_t *pid_out, void *stack) {
        (void) tcb;

        long ret = syscall2(SYS_THREADNEW, (long) __mlibc_thread_entry, (long) stack);
        if (ret < 0) {
            return -ret;
        }

        *pid_out = ret;
        return 0;
    }

    [[noreturn]] void Sysdeps<ThreadExit>::operator()(void) {
        syscall0(SYS_THREADEXIT);
        __builtin_unreachable();
    }

#endif

    int Sysdeps<FutexTid>::operator()(void) {
        return syscall0(SYS_GETTID);
    }

    #define FUTEX_WAIT 0
    #define FUTEX_WAKE 1

    int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
        (void) time;

        long ret = syscall3(SYS_FUTEX, (long) pointer, FUTEX_WAIT, expected);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
        long ret = syscall3(SYS_FUTEX, (long) pointer, FUTEX_WAKE, all ? UINT32_MAX : 1);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Uname>::operator()(struct utsname *buf) {
        long ret = syscall1(SYS_UNAME, (long) buf);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<Pause>::operator()(void) {
        sigset_t mask;

        int ret = 0;
        if ((ret = sysdep<Sigprocmask>(0, NULL, &mask)) != 0) {
            return ret;
        }

        return sysdep<Sigsuspend>(&mask);
    }

    int Sysdeps<TcbSet>::operator()(void *pointer) {
        long ret = syscall2(SYS_ARCHCTL, ARCHCTL_SET_FSBASE, (long) pointer);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
        long ret = syscall6(SYS_MMAP, (long) hint, size, prot, flags, fd, offset);
        if (ret < 0 && ret >= -4095) {
            return -ret;
        }

        *window = (void*) ret;
        return 0;
    }

    int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
        long ret = syscall2(SYS_MUNMAP, (long) pointer, size);
        if (ret < 0) {
            return -ret;
        }
        return 0;
	}

    int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
        long ret = syscall3(SYS_MPROTECT, (long) pointer, size, prot);
        if (ret < 0) {
            return -ret;
        }
        return 0;
    }

    int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
        size += 4096 - (size % 4096);
        return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0, pointer);
    }

    int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
        size += 4096 - (size % 4096);
        return sysdep<VmUnmap>(pointer, size);
    }

    void Sysdeps<LibcLog>::operator()(const char *message) {
        long ret;
        sysdep<Write>(2, message, strlen(message), &ret);
        sysdep<Write>(2, "\n", 1, &ret);
    }

    [[noreturn]] void Sysdeps<LibcPanic>::operator()() {
        sysdep<LibcLog>("mlibc: panic");
        sysdep<Exit>(1);
    }

    int Sysdeps<Access>::operator()(const char *path, int mode) {
        (void) path;
        (void) mode;
        return 0;
    }

    uid_t Sysdeps<GetUid>::operator()(void) {
        return 0;
    }

    uid_t Sysdeps<GetEuid>::operator()(void) {
        return 0;
    }

    gid_t Sysdeps<GetGid>::operator()(void) {
        return 0;
    }

    gid_t Sysdeps<GetEgid>::operator()(void) {
        return 0;
    }

    int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
        *ruid = *euid = *suid = 0;
        return 0;
    }

    int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
        *rgid = *egid = *sgid = 0;
        return 0;
    }
} // namespace mlibc
