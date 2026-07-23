#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/socket.h>
#include <sys/ioctl_ethereal.h>
#include <sched.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/file.h>
#include <mlibc/tcb.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stddef.h>

namespace mlibc {
void Sysdeps<LibcLog>::operator()(const char *message) {
    ssize_t unused;
    sysdep<Write>(2, message, strlen(message), &unused);
    sysdep<Write>(2, "\n", 1, &unused);
}

void Sysdeps<LibcPanic>::operator()() {
    sysdep<LibcLog>("\n\033[0;31mINTERNAL ERROR:\033[0m mlibc panic detected!\n");
    sysdep<Exit>(1);
}

void Sysdeps<Exit>::operator()(int status) {
    SYSCALL1(SYS_EXIT, status);
    sysdep<LibcPanic>();
    __builtin_unreachable();
}

int Sysdeps<Read>::operator()(int fd, void *buf, size_t n, ssize_t *bytes_read) {
    long err = SYSCALL3(SYS_READ, fd, buf, n);

    if (err < 0) {
        return -err;
    }

    *bytes_read = err;
    return 0; 
}

int Sysdeps<Write>::operator()(int fd, const void *buf, size_t n, ssize_t *bytes_written) {
    ssize_t err = SYSCALL3(SYS_WRITE, fd, buf, n);

    if (err < 0) {
        return -err;
    }

    *bytes_written = err;
    return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
    long error = SYSCALL3(SYS_LSEEK, fd, offset, whence);

    if (error < 0) {
        return -error;
    }

    *new_offset = error;
    return 0;
}

int Sysdeps<Open>::operator()(const char *pathname, int flags, mode_t mode, int *fd) {
    int error = SYSCALL3(SYS_OPEN, pathname, flags, mode);
    
    if (error < 0) {
        return -error;
    }

    *fd = error;
    return 0;
}

int Sysdeps<Close>::operator()(int fd) {
    return -SYSCALL1(SYS_CLOSE, fd);
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
    return -SYSCALL2(SYS_MKDIR, path, mode);
}

int Sysdeps<VmMap>::operator()(void *addr, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
    struct {  
        void *addr;
        size_t len;
        int prot;
        int flags;
        int filedes;
        off_t off;
    } ctx;

    ctx.addr = addr;
    ctx.len = size;
    ctx.prot = prot;
    ctx.flags = flags;
    ctx.filedes = fd;
    ctx.off = offset;

    long ret = SYSCALL1(SYS_MMAP, &ctx);
    if (ret < 0) {
        *window = MAP_FAILED;
        return -(ret);
    }

    *window = (void*)ret;
    return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
    return -SYSCALL2(SYS_MUNMAP, pointer, size);
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
    return -SYSCALL3(SYS_MPROTECT, pointer, size, prot);
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
    return sysdep<VmMap>(NULL, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0, pointer);
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
    return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
    return -SYSCALL1(SYS_SETTLS, (uintptr_t)pointer);
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
    return -SYSCALL3(SYS_CLOCK_GETTIME, clock, secs, nanos);
}

int Sysdeps<Times>::operator()(struct tms *tms, clock_t *out) {
    long err = SYSCALL1(SYS_TIMES, tms);
    if (err < 0) return -err;
    *out = (clock_t)err;
    return 0;
}

int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags) {
    return -SYSCALL4(SYS_FACCESSAT, dirfd, pathname, mode, flags);
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
    return sysdep<Faccessat>(AT_FDCWD, path, mode, 0);
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
    long error = SYSCALL3(SYS_WAIT, pid, status, flags);

    if (error < 0)  {
        return -error;
    }

    *ret_pid = (pid_t)error;
    return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
    long error = SYSCALL3(SYS_EXECVE, path, argv, envp);
    if (error < 0) {
        return -error;
    }

    return 0;
}

int Sysdeps<Fork>::operator()(pid_t *child) {
    long error = SYSCALL0(SYS_FORK);
    if (error < 0) return -error;
    *child = error;
    return 0;
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
    long err = SYSCALL3(SYS_IOCTL, fd, request, arg);
    if (err < 0) {
        return -err;
    }

    *result = err;
    return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
    int is_tty = 0;
    long error = SYSCALL3(SYS_IOCTL, fd, IOCTLTTYIS, &is_tty);
    if (error < 0) return ENOTTY;
    return 0;
}

int Sysdeps<Ptsname>::operator()(int fd, char *buffer, size_t length) {
    long error = SYSCALL3(SYS_IOCTL, fd, IOCTLTTYNAME, buffer);
    if (error < 0) return -error;
    return 0;
}

int Sysdeps<Ttyname>::operator()(int fd, char *buffer, size_t size) {
    // !!!: This is wrong
    return sysdep<Ptsname>(fd, buffer, size);
}

int Sysdeps<Pread>::operator()(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
    ssize_t s = SYSCALL4(SYS_PREAD, fd, buf, n, off);
    if (s < 0) {
        return -s;
    }

    *bytes_read = s;
    return 0;
}

int Sysdeps<Pwrite>::operator()(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
    ssize_t s = SYSCALL4(SYS_PWRITE, fd, buf, n, off);
    if (s < 0) {
        return -s;
    }

    *bytes_written = s;
    return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
    (void)flags;
    long error = SYSCALL2(SYS_DUP2, fd, newfd);
    if (error < 0) return -error;
    return 0;
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
    (void)flags;
    int ret = SYSCALL2(SYS_DUP2, fd, -1);
    if (ret < 0) return -ret;
    *newfd = ret;
    return 0;
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
    long error = SYSCALL3(SYS_POLL, fds, count, timeout);
    if (error < 0) return -error;
    *num_events = error;
    return 0;
}

int Sysdeps<Pselect>::operator()(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
    struct {
        int nfds;
        fd_set *readfds;
        fd_set *writefds;
        fd_set *errorfds;
        const struct timespec *timeout;
        const sigset_t *sigmask;
    } ctx;

    ctx.nfds = num_fds;
    ctx.readfds = read_set;
    ctx.writefds = write_set;
    ctx.errorfds = except_set;
    ctx.timeout = timeout;
    ctx.sigmask = sigmask;

    long err = SYSCALL1(SYS_PSELECT, &ctx);
    if (err < 0) return -err;
    *num_events = err;
    return 0;
}

gid_t Sysdeps<GetGid>::operator()() { return SYSCALL0(SYS_GETGID); }
gid_t Sysdeps<GetEgid>::operator()() { return SYSCALL0(SYS_GETEGID); }
uid_t Sysdeps<GetUid>::operator()() { return SYSCALL0(SYS_GETUID); }
uid_t Sysdeps<GetEuid>::operator()() { return SYSCALL0(SYS_GETEUID); }
pid_t Sysdeps<GetPid>::operator()() { return SYSCALL0(SYS_GETPID); }
pid_t Sysdeps<GetTid>::operator()() { return SYSCALL0(SYS_GETTID); }
pid_t Sysdeps<GetPpid>::operator()() { return SYSCALL0(SYS_GETPPID); }
pid_t Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
    long err = SYSCALL1(SYS_GETPGID, pid);
    if (err < 0) return -err;
    *pgid = err;
    return 0;
}

int Sysdeps<SetUid>::operator()(uid_t uid) { return -SYSCALL1(SYS_SETUID, uid); }
int Sysdeps<SetEuid>::operator()(uid_t euid) { return -SYSCALL1(SYS_SETEUID, euid); }
int Sysdeps<SetGid>::operator()(gid_t gid) { return -SYSCALL1(SYS_SETGID, gid); }
int Sysdeps<SetEgid>::operator()(gid_t egid) { return -SYSCALL1(SYS_SETEGID, egid); }

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
    long err = SYSCALL2(SYS_SETPGID, pid, pgid);
    return -err;
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
    long error = SYSCALL0(SYS_SETSID);
    if (error < 0) return -error;
    *sid = error;
    return 0;
}

void Sysdeps<Yield>::operator()() {
    SYSCALL0(SYS_YIELD);
}

int Sysdeps<Pause>::operator()() {
    return -SYSCALL0(SYS_PAUSE);
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const struct timespec *time) {
    long ret = SYSCALL3(SYS_FUTEX_WAIT, pointer, expected, time);
    if (ret < 0) return -ret;
    return 0;
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
    long ret = SYSCALL2(SYS_FUTEX_WAKE, pointer, all ? INT_MAX : 1);
    if (ret < 0) return -ret;
    return 0;
}

int Sysdeps<GetCwd>::operator()(char *buffer, size_t size) {
    memset(buffer, 0, size);
    long err = SYSCALL2(SYS_GETCWD, buffer, size);
    if (err < 0) return -(err);
    return 0;
}

int Sysdeps<Chdir>::operator()(const char *path) {
    return -SYSCALL1(SYS_CHDIR, path);
}

int Sysdeps<Fchdir>::operator()(int fd) {
    return -SYSCALL1(SYS_FCHDIR, fd);
}

int Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size, ssize_t *length) {
    long err = SYSCALL3(SYS_READLINK, path, buffer, max_size);
    if (err < 0) return -err;
    *length = err;
    return 0;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
    return sysdep<Open>(path, O_DIRECTORY, 0, handle);
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
    long err = SYSCALL3(SYS_READ_ENTRIES, handle, buffer, max_size);
    if (err < 0) return -(err);
    
    *bytes_read = err;
    return 0;
}

int Sysdeps<Stat>::operator()(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
    if (fsfdt == fsfd_target::fd) {
        // Targeting file descriptor, fstat.
        return -(SYSCALL2(SYS_FSTAT, fd, statbuf));
    } else if (fsfdt == fsfd_target::path) {
        if (flags & AT_SYMLINK_NOFOLLOW) {
            return -(SYSCALL2(SYS_LSTAT, path, statbuf));
        } else {
            return -(SYSCALL2(SYS_STAT, path, statbuf));
        }
    } else if (fsfdt == fsfd_target::fd_path) {
        return -SYSCALL4(SYS_FSTATAT, fd, path, statbuf, flags);
    } else {
        mlibc::panicLogger() << "mlibc: fsfd_target is invalid" << frg::endlog;
        __builtin_unreachable();
    }
}

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
    long err = SYSCALL3(SYS_SOCKET, family, type, protocol);
    if (err < 0) return -err;
    *fd = err;
    return 0;
}


int Sysdeps<MsgSend>::operator()(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
    long err = SYSCALL3(SYS_SENDMSG, fd, hdr, flags);
    if (err < 0) return -err;
    *length = err;
    return 0;
}

int Sysdeps<MsgRecv>::operator()(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
    long err = SYSCALL3(SYS_RECVMSG, fd, hdr, flags);
    if (err < 0) return -err;
    *length = err;
    return 0;
}

int Sysdeps<Socketpair>::operator()(int domain, int type_and_flags, int proto, int *fds) {
    long err = SYSCALL4(SYS_SOCKETPAIR, domain, type_and_flags, proto, fds);
    return -err;
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
    long err = SYSCALL2(SYS_LISTEN, fd, backlog);
    return -(err);
}

int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
    long err = SYSCALL5(SYS_GETSOCKOPT, fd, layer, number, buffer, size);
    return -err;
} 
    
int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
    struct sockopt_params {
        int socket;
        int level;
        int option_name;
        const void *option_value;
        socklen_t option_len;
    } params;

    params.socket = fd;
    params.level = layer;
    params.option_name = number;
    params.option_value = buffer;
    params.option_len = size;

    return -(SYSCALL1(SYS_SETSOCKOPT, &params));
}


int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
    long err = (SYSCALL3(SYS_ACCEPT, fd, addr_ptr, addr_length));
    if (err < 0) return -err;
    *newfd = err;
    return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    long err = (SYSCALL3(SYS_BIND, fd, addr_ptr, addr_length));
    return -err;
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    long err = SYSCALL3(SYS_CONNECT, fd, addr_ptr, addr_length);
    return -err;
}

int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
    socklen_t len = max_addr_length;
    long err = SYSCALL3(SYS_GETSOCKNAME, fd, addr_ptr, &len);
    if (err < 0) return -err;
    *actual_length = len;
    return 0;
}

int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length) {
    socklen_t len = max_addr_length;
    long err = SYSCALL3(SYS_GETPEERNAME, fd, addr_ptr, &len);
    if (err < 0) return -err;
    *actual_length = len;
    return 0;
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
    useconds_t usec = (*secs * 1000000) + (*nanos/1000);
    SYSCALL1(SYS_USLEEP, usec);
    return 0;
}

int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
    long err = SYSCALL3(SYS_SIGPROCMASK, how, set, retrieve);
    return -err;
}

int Sysdeps<Sigaction>::operator()(int signum, const struct sigaction *__restrict act,
    struct sigaction *__restrict oact) {
    long err = SYSCALL3(SYS_SIGACTION, signum, act, oact);
    return -err;
}

int Sysdeps<Kill>::operator()(int pid, int sig) {
    long err = SYSCALL2(SYS_KILL, pid, sig);
    return -err;
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
    return -SYSCALL3(SYS_IOCTL, fd, TCGETS, attr);
}

int Sysdeps<Tcsetattr>::operator()(int fd, int optional, const struct termios *attr) {
    switch (optional) {
        case TCSADRAIN:
            return -SYSCALL3(SYS_IOCTL, fd, TCSETSW, (void*)attr);
        case TCSAFLUSH:
            return -SYSCALL3(SYS_IOCTL, fd, TCSETSF, (void*)attr);
        case TCSANOW:
        default:
            return -SYSCALL3(SYS_IOCTL, fd, TCSETS, (void*)attr);
    }
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
    long ret = SYSCALL1(SYS_PIPE, fds);
    return -ret;
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
    return -(SYSCALL2(SYS_GETHOSTNAME, buffer, bufsize));
}

int Sysdeps<SetHostname>::operator()(const char *buffer, size_t bufsize) {
    return -(SYSCALL2(SYS_SETHOSTNAME, buffer, bufsize));
}

int Sysdeps<Ftruncate>::operator()(int fd, size_t size) {
    return -SYSCALL2(SYS_FTRUNCATE, fd, size);
}

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
    int r = SYSCALL3(SYS_FCNTL, fd, request, va_arg(args, uint64_t));
    if (r < 0) return -r;
    *result = r;
    return 0;
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
    return -(SYSCALL1(SYS_UNAME, buf));
}

int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
    long ret = SYSCALL1(SYS_UMASK, mode);
    if (ret < 0) {
        return -ret;
    }

    *old = (mode_t)ret;
    return 0;
}

int Sysdeps<Fsync>::operator()(int fd) {
    return -(SYSCALL1(SYS_FSYNC, fd));
}

void Sysdeps<Sync>::operator()() {
    SYSCALL0(SYS_SYNC);
}

int Sysdeps<Fchmodat>::operator()(int dirfd, const char *pathname, mode_t mode, int flags) {
    return -SYSCALL4(SYS_FCHMODAT, dirfd, pathname, mode, flags);
}

int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
    return sysdep<Fchmodat>(fd, "", mode, AT_EMPTY_PATH);
}

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
    return sysdep<Fchmodat>(AT_FDCWD, pathname, mode, 0);
}

int Sysdeps<Fchownat>::operator()(int dirfd, const char *pathname, uid_t uid, gid_t gid, int flags) {
    return -SYSCALL5(SYS_FCHOWNAT, dirfd, pathname, uid, gid, flags);
}

int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
    int r = SYSCALL4(SYS_OPENAT, dirfd, path, flags, mode);
    if (r < 0) return -r;
    *fd = r;
    return 0;
}

#ifndef MLIBC_BUILDING_RTLD
int Sysdeps<GetEntropy>::operator()(void *buffer, size_t length) {
    int fd;
    if (sysdep<Open>("/device/random", O_RDONLY, 0, &fd)) {
        mlibc::panicLogger() << "mlibc: /device/random: " << strerror(errno) << frg::endlog;
    }

    ssize_t bytes;
    int err = sysdep<Read>(fd, buffer, length, &bytes); 
    if (err) {
        mlibc::infoLogger() << "mlibc: reading from /device/random failed: " << strerror(errno) << frg::endlog;
        return err;
    }

    sysdep<Close>(fd);
    return 0;
}
#endif

int Sysdeps<Unlinkat>::operator()(int fd, const char *path, int flags) {
    long ret = SYSCALL3(SYS_UNLINKAT, fd, path, flags);
    return -ret;
}

int Sysdeps<Rmdir>::operator()(const char *path) {
    return sysdep<Unlinkat>(AT_FDCWD, path, AT_REMOVEDIR);
}

int Sysdeps<Renameat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
    return -SYSCALL5(SYS_RENAMEAT, olddirfd, old_path, newdirfd, new_path, 0);
}

int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
    return sysdep<Renameat>(AT_FDCWD, path, AT_FDCWD, new_path);
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
    return -SYSCALL3(SYS_SYMLINKAT, target_path, dirfd, link_path);
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
    return sysdep<Symlinkat>(target_path, AT_FDCWD, link_path);
}

int Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
    // TODO
    return sysdep<Symlinkat>(old_path, newdirfd, new_path);
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
    return sysdep<Linkat>(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int Sysdeps<SetItimer>::operator()(int which, const struct itimerval *new_value, struct itimerval *old_value) {
    long ret = SYSCALL3(SYS_SETITIMER, which, new_value, old_value);
    return ret;
}

int Sysdeps<Openpty>::operator()(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win) {
    return -SYSCALL5(SYS_OPENPTY, mfd, sfd, name, ios, win);
}

#ifndef MLIBC_BUILDING_RTLD
extern "C" void __mlibc_thread_entry();
int Sysdeps<Clone>::operator()(void *tcb, pid_t *pid_out, void *stack) {
    long ret = SYSCALL4(SYS_CREATE_THREAD, (uintptr_t)stack, (uintptr_t)tcb, (void*)__mlibc_thread_entry, NULL);
    if (ret < 0) return -ret;

    *pid_out = (pid_t)ret;
    return 0;
}

#endif
}
