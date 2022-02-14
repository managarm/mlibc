
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <qword/fcntl.h>
#include <limits.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {

int sys_gethostname(char *buffer, size_t bufsize) {
    int ret;
    asm volatile ("syscall" : "=a"(ret)
            : "a"(36), "D"(buffer), "S"(bufsize)
            : "rcx", "r11", "rdx");
    return ret;
}

void sys_libc_log(const char *message) {
	unsigned long res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(0), "D"(0), "S"(message)
			: "rcx", "r11", "rdx");
}

void sys_libc_panic() {
    mlibc::infoLogger() << "\e[31mmlibc: panic!" << frg::endlog;
    asm volatile ("syscall" :
            : "a"(12), "D"(1)
            : "rcx", "r11", "rdx");
}

int sys_getcwd(char *buffer, size_t size) {
    int ret;
    int sys_errno;

	asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
			: "a"(20), "D"(buffer), "S"(size)
			: "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_chdir(const char *path) {
    int ret;
    int sys_errno;

	asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
			: "a"(15), "D"(path)
			: "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_tcb_set(void *pointer) {
    int res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(7), "D"(pointer)
			: "rcx", "r11", "rdx");
    return res;
}

int sys_anon_allocate(size_t size, void **pointer) {
	// The qword kernel wants us to allocate whole pages.
	__ensure(!(size & 0xFFF));

	void *ret;
    int sys_errno;

	asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
			: "a"(6), "D"(0), "S"(size >> 12)
			: "rcx", "r11");

    if (!ret)
        return sys_errno;

	*pointer = ret;
    return 0;
}

int sys_anon_free(void *pointer, size_t size) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
	return 0;
}

#ifndef MLIBC_BUILDING_RTDL
void sys_exit(int status) {
    asm volatile ("syscall" :
            : "a"(12), "D"(status)
            : "rcx", "r11", "rdx");
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_clock_get(int clock, time_t *secs, long *nanos) {
    int ret;
    int sys_errno;
    struct timespec tp = {0};
    asm volatile("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(25), "D"(clock), "S"(&tp)
            : "rcx", "r11");
    if (ret == -1)
        return sys_errno;

    *secs = tp.tv_sec;
    *nanos = tp.tv_nsec;
    return 0;
}
#endif

int sys_open(const char *path, int flags, int *fd) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(1), "D"(path), "S"(flags), "d"(0)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *fd = ret;
    return 0;
}

int sys_close(int fd) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(2), "D"(fd)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    ssize_t ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(3), "D"(fd), "S"(buf), "d"(count)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *bytes_read = ret;
    return 0;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
    ssize_t ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(4), "D"(fd), "S"(buf), "d"(count)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *bytes_written = ret;
    return 0;
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
#endif

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    off_t ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(8), "D"(fd), "S"(offset), "d"(whence)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *new_offset = ret;
    return 0;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags,
		int fd, off_t offset, void **window) {
    __ensure(flags & MAP_ANONYMOUS);
    void *res;
    size_t size_in_pages = (size + 4096 - 1) / 4096;
    asm volatile ("syscall" : "=a"(res)
            : "a"(6), "D"(hint), "S"(size_in_pages)
            : "rcx", "r11");
    if(!res)
        return -1; // TODO: Properly report error.
    *window = res;
    return 0;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window) STUB_ONLY
#endif // !defined(MLIBC_BUILDING_RTDL)

int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY

#ifndef MLIBC_BUILDING_RTDL
namespace {
	int do_fstat(int fd, struct stat *statbuf) {
		int ret;
		int sys_errno;

		asm volatile ("syscall"
				: "=a"(ret), "=d"(sys_errno)
				: "a"(9), "D"(fd), "S"(statbuf)
				: "rcx", "r11");

		if (ret == -1)
			return sys_errno;

		return 0;
	}
}

int sys_unlink(const char *path) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(34), "D"(path)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_unlinkat(int fd, const char *path, int flags) {
    __ensure(flags == 0);
    if (path[0] == '/') {
        // The path is absolute, remove
        return sys_unlink(path);
    }
    // FIXME: this part can totally buffer overflow
    char fd_path[PATH_MAX];
    if (fd == AT_FDCWD) {
        int e = sys_getcwd(fd_path, PATH_MAX);
        if (e)
            return e;
    } else {
        int e = fcntl(fd, F_GETPATH, fd_path);
        if (e)
            return errno;
    }
    strcat(fd_path, "/");
    strcat(fd_path, path);
    return sys_unlink(fd_path);
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	if (fsfdt == fsfd_target::fd) {
		return do_fstat(fd, statbuf);
	} else if (fsfdt == fsfd_target::path) {
_stat:
		int e = sys_open(path, 0/*O_RDONLY*/, &fd);
		if (e)
			return e;
		e = do_fstat(fd, statbuf);
		sys_close(fd);
		return e;
	} else {
		__ensure(fsfdt == fsfd_target::fd_path);
        if (path[0] == '/') {
            // The path is absolute, open normally
            goto _stat;
        }
        // FIXME: this part can totally buffer overflow
        char fd_path[PATH_MAX];
        if (fd == AT_FDCWD) {
            int e = sys_getcwd(fd_path, PATH_MAX);
            if (e)
                return e;
        } else {
            int e = fcntl(fd, F_GETPATH, fd_path);
            if (e)
                return errno;
        }
        strcat(fd_path, "/");
        strcat(fd_path, path);
        path = fd_path;
        goto _stat;
    }
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_rename(const char *path, const char *new_path) STUB_ONLY
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(29), "D"(signum), "S"(act), "d"(oldact)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
#endif

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(32), "D"(pointer), "S"(expected)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_futex_wake(int *pointer) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(33), "D"(pointer)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTDL

int sys_open_dir(const char *path, int *handle) {
	return sys_open(path, 0, handle);
}
int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(17), "D"(handle), "S"(buffer)
            : "rcx", "r11");

    if (ret == -1 && sys_errno == 0) {
        /* end of dir */
        *bytes_read = 0;
        return 0;
    } else if (ret == -1) {
        return sys_errno;
    }

    *bytes_read = sizeof(struct dirent);
    return 0;
}

int sys_access(const char *path, int mode) {
	int fd;
	if(int e = sys_open(path, O_RDWR, &fd); e)
		return e;
	sys_close(fd);
	return 0;
}

static int internal_dup(int fd, int *newfd, ...) {
    va_list l;
    va_start(l, newfd);
    int ret;
    int sys_errno = sys_fcntl(fd, F_DUPFD, l, &ret);
    va_end(l);
    if (sys_errno)
        return sys_errno;
    *newfd = ret;
    return 0;
}

int sys_dup(int fd, int flags, int *newfd) {
    (void)flags;
    int ret;
    int sys_errno = internal_dup(fd, &ret, 0);
    if (sys_errno)
        return sys_errno;
    *newfd = ret;
    return 0;
}

int sys_dup2(int fd, int flags, int newfd) {
    (void)flags;
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(16), "D"(fd), "S"(newfd)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_isatty(int fd) {
    int ret;

    asm volatile ("syscall"
            : "=a"(ret)
            : "a"(31), "D"(fd)
            : "rcx", "r11", "rdx");

    if (ret)
        return 0;
    else
        return ENOTTY;
}
int sys_ttyname(int fd, char *buf, size_t size) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
int sys_chroot(const char *path) STUB_ONLY
int sys_mkdir(const char *path) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(35), "D"(path)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_tcgetattr(int fd, struct termios *attr) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(24), "D"(fd), "S"(attr)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(23), "D"(fd), "S"(optional_action), "d"(attr)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_tcflow(int fd, int action) {
    int ret;
    int sys_errno;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(30), "D"(fd), "S"(action)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_pipe(int *fds, int flags) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(19), "D"(fds), "S"(flags)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}
int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) STUB_ONLY
int sys_ftruncate(int fd, size_t size) STUB_ONLY
int sys_fallocate(int fd, off_t offset, size_t size) STUB_ONLY
int sys_symlink(const char *target_path, const char *link_path) STUB_ONLY
int sys_fcntl(int fd, int request, va_list args, int *result) {
    int ret;
    int sys_errno;
    size_t arg = va_arg(args, size_t);

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(18), "D"(fd), "S"(request), "d"(arg)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *result = ret;
    return 0;
}

int sys_socket(int family, int type, int protocol, int *fd) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(44), "D"(family), "S"(type), "d"(protocol)
            : "rcx", "r11");

    if (ret < 0)
        return sys_errno;
    *fd = ret;
    return 0;
}
int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) STUB_ONLY
int sys_accept(int fd, int *newfd) STUB_ONLY
int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(45), "D"(fd), "S"(addr_ptr), "d"(addr_length)
            : "rcx", "r11");

    if (ret < 0)
        return sys_errno;
    return ret; // bind returns 0 on success
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) {
    ssize_t ret;
    int sys_errno;

    struct iovec *iov = hdr->msg_iov;
    const void *buf = iov->iov_base;
    size_t len = iov->iov_len;


    struct sockaddr *src_addr = (sockaddr *)hdr->msg_name;
    socklen_t addrlen = hdr->msg_namelen;

    if (src_addr) {
        register int arg_r10 asm("r10") = flags;
        register const struct sockaddr *arg_r9 asm("r9") = src_addr;
        register socklen_t arg_r8 asm("r8") = addrlen;

        asm volatile ("syscall"
                : "=a"(ret), "=d"(sys_errno)
                : "a"(46), "D"(fd), "S"(buf), "d"(len), "r"(arg_r10), "r"(arg_r9), "r"(arg_r8)
                : "rcx", "r11");
        if (ret < 0)
            return sys_errno;
        *length = ret;
        return 0;
    }

    register int arg_r10 asm("r10") = flags;
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(49), "D"(fd), "S"(buf), "d"(len), "r"(arg_r10)
            : "rcx", "r11");
    if (ret < 0)
        return sys_errno;
    *length = ret;
    return 0;
}

int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) {
    ssize_t ret;
    int sys_errno;


    struct iovec *iov = hdr->msg_iov;
    void *buf = iov->iov_base;
    size_t len = iov->iov_len;

    struct sockaddr *src_addr = (sockaddr *)hdr->msg_name;
    socklen_t *addrlen = &hdr->msg_namelen;

    if (src_addr) {
        register struct sockaddr *arg_r10 asm("r10") = src_addr;
        register socklen_t *arg_r8 asm("r8") = addrlen;

        asm volatile ("syscall"
                : "=a"(ret), "=d"(sys_errno)
                : "a"(47), "D"(fd), "S"(buf), "d"(len), "r"(arg_r10), "r"(arg_r8)
                : "rcx", "r11");
        if (ret < 0)
            return sys_errno;
        *length = ret;
        return 0;
    }

    // if addr not specified, use recv()
    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(48), "D"(fd), "S"(buf), "d"(flags)
            : "rcx", "r11");
    if (ret < 0)
        return sys_errno;
    *length = ret;
    return 0;
}
int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) STUB_ONLY
int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) STUB_ONLY
int sys_setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) STUB_ONLY

int sys_sleep(time_t *secs, long *nanos) {
    asm volatile ("syscall"
        :
        : "a"(40), "D"(*secs)
        : "rcx", "r11");
    *secs = 0;
    *nanos = 0;
    return 0;
}
int sys_fork(pid_t *child) {
    pid_t ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(10)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *child = ret;
    return 0;
}
int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(11), "D"(path), "S"(argv), "d"(envp)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_kill(pid_t pid, int signal) {
    int ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(27), "D"(pid), "S"(signal)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) {
    pid_t ret;
    int sys_errno;

    asm volatile ("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(13), "D"(pid), "S"(status), "d"(flags)
            : "rcx", "r11");

    if (ret == -1)
        return sys_errno;

    *ret_pid = ret;
    return 0;
}

int sys_getrusage(int who, struct rusage *usage) {
    int ret;
    int sys_errno;

    asm volatile("syscall"
            : "=a"(ret), "=d"(sys_errno)
            : "a"(26), "D"(who), "S"(usage)
            : "rcx", "r11");
    if (ret == -1)
        return sys_errno;
    return 0;
}

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
void sys_yield() STUB_ONLY

gid_t sys_getgid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
gid_t sys_getegid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
uid_t sys_getuid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
uid_t sys_geteuid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
pid_t sys_getpid() {
    pid_t pid;
    asm volatile ("syscall" : "=a"(pid)
            : "a"(5)
            : "rcx", "r11", "rdx");
    return pid;
}
pid_t sys_getppid() {
    pid_t ppid;
    asm volatile ("syscall" : "=a"(ppid)
            : "a"(14)
            : "rcx", "r11", "rdx");
    return ppid;
}

pid_t sys_getpgid(pid_t pid) {
    if(pid != 0)
        return ENOSYS;
    pid_t pgid;
    asm volatile ("syscall" : "=a"(pgid)
            : "a"(38), "D"(0)
            : "rcx", "r11", "rdx");
    return pgid;
}

int sys_setuid(uid_t uid) {
    int ret;
    asm volatile ("syscall" : "=a" (ret)
            : "a"(39), "D"(uid)
            : "rcx", "r11", "rdx");
    return ret;
}

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
		const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	mlibc::infoLogger() << "mlibc: sys_pselect is a stub!" << frg::endlog;
	return 0;
}

#endif // MLIBC_BUILDING_RTDL

} // namespace mlibc

