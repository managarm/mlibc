
#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>
#include <errno.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {

void sys_libc_log(const char *message) {
	unsigned long res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(0), "D"(0), "S"(message)
			: "rcx", "r11");
}

void sys_libc_panic() {
    mlibc::infoLogger() << "\e[31mmlibc: sys_libc_panic() called!" << frg::endlog;
    for (;;);
}

int sys_tcb_set(void *pointer) {
    int res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(7), "D"(pointer)
			: "rcx", "r11");
    return res;
}

int sys_anon_allocate(size_t size, void **pointer) {
	// The qword kernel wants us to allocate whole pages.
	__ensure(!(size & 0xFFF));

	void *res;
	asm volatile ("syscall" : "=a"(res)
			: "a"(6), "D"(0), "S"(size >> 12)
			: "rcx", "r11");
	if(!res)
		return ENOMEM;
	*pointer = res;
	return 0;
}

int sys_anon_free(void *pointer, size_t size) STUB_ONLY

#ifndef MLIBC_BUILDING_RTDL
void sys_exit(int status) {
    mlibc::infoLogger() << "\e[31mmlibc: sys_exit() called (not implemented yet!)" << frg::endlog;
    for (;;);
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_clock_get(int clock, time_t *secs, long *nanos) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
#endif

int sys_open(const char *path, int flags, int *fd) {
    // For now, we ignore the flags.
    // TODO: Adjust the ABI so that the flags match qword kernel flags.
    flags = 0;

    int res;
    asm volatile ("syscall" : "=a"(res)
            : "a"(1), "D"(path), "S"(0), "d"(flags)
            : "rcx", "r11");
    if(res < 0)
        return -1; // TODO: Properly report error.
    *fd = res;
    return 0;
}

int sys_close(int fd) {
    int res;
    asm volatile ("syscall" : "=a"(res)
            : "a"(2), "D"(fd)
            : "rcx", "r11");
    if(res < 0)
        return -1; // TODO: Properly report error.
    return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
    ssize_t res;
    asm volatile ("syscall" : "=a"(res)
            : "a"(3), "D"(fd), "S"(buf), "d"(count)
            : "rcx", "r11");
    *bytes_read = res;
    return 0;
}

#ifndef MLIBC_BUILDING_RTDL
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
    ssize_t res;
    asm volatile ("syscall" : "=a"(res)
            : "a"(4), "D"(fd), "S"(buf), "d"(count)
            : "rcx", "r11");
    *bytes_written = res;
    return 0;
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
#endif

#define __QWORD_SEEK_SET        0
#define __QWORD_SEEK_CUR        1
#define __QWORD_SEEK_END        2

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
    off_t res;
    int qword_type;
    switch (whence) {
        case SEEK_SET:
            qword_type = __QWORD_SEEK_SET;
            break;
        case SEEK_CUR:
            qword_type = __QWORD_SEEK_CUR;
            break;
        case SEEK_END:
            qword_type = __QWORD_SEEK_END;
            break;
        default:
            return EINVAL;
    }
    asm volatile ("syscall" : "=a"(res)
            : "a"(8), "D"(fd), "S"(offset), "d"(qword_type)
            : "rcx", "r11");
    if(res < 0)
        return -1; // TODO: Properly report error.
    *new_offset = res;
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
int sys_fstat(int fd, struct stat *statbuf) {
    int res;
    asm volatile ("syscall" : "=a"(res)
            : "a"(9), "D"(fd), "S"(statbuf)
            : "rcx", "r11");
    if(res < 0)
        return -1; // TODO: Properly report error.
    return 0;
}
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_rename(const char *path, const char *new_path) STUB_ONLY
#endif

#ifndef MLIBC_BUILDING_RTDL
int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
#endif

// All remaining functions are disabled in ldso.
#ifndef MLIBC_BUILDING_RTDL

int sys_futex_wait(int *pointer, int expected) STUB_ONLY
int sys_futex_wake(int *pointer) STUB_ONLY

int sys_open_dir(const char *path, int *handle) STUB_ONLY
int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) STUB_ONLY

int sys_access(const char *path, int mode) STUB_ONLY
int sys_dup(int fd, int flags, int *newfd) STUB_ONLY
int sys_dup2(int fd, int flags, int newfd) STUB_ONLY
int sys_isatty(int fd) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
int sys_ttyname(int fd, char *buf, size_t size) STUB_ONLY
int sys_stat(const char *path, struct stat *statbuf) {
    int fd;
    if (sys_open(path, 0/*O_RDONLY*/, &fd) < 0)
        return -1; // TODO: Properly report error.
    int ret = sys_fstat(fd, statbuf);
    if (ret < 0) {
        sys_close(fd);
        return -1; // TODO: Properly report error.
    }
    sys_close(fd);
    return ret;
}
int sys_lstat(const char *path, struct stat *statbuf) STUB_ONLY
int sys_chroot(const char *path) STUB_ONLY
int sys_mkdir(const char *path) STUB_ONLY
int sys_tcgetattr(int fd, struct termios *attr) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
int sys_tcsetattr(int, int, const struct termios *attr) STUB_ONLY
int sys_pipe(int *fds) STUB_ONLY
int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length) STUB_ONLY
int sys_ftruncate(int fd, size_t size) STUB_ONLY
int sys_fallocate(int fd, off_t offset, size_t size) STUB_ONLY
int sys_unlink(const char *path) STUB_ONLY
int sys_symlink(const char *target_path, const char *link_path) STUB_ONLY
int sys_fcntl(int fd, int request, va_list args, int *result) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}

int sys_socket(int family, int type, int protocol, int *fd) STUB_ONLY
int sys_socketpair(int domain, int type_and_flags, int proto, int *fds) STUB_ONLY
int sys_accept(int fd, int *newfd) STUB_ONLY
int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length) STUB_ONLY
int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length) STUB_ONLY
int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) STUB_ONLY
int sys_getsockopt(int fd, int layer, int number,
		void *__restrict buffer, socklen_t *__restrict size) STUB_ONLY
int sys_setsockopt(int fd, int layer, int number,
		const void *buffer, socklen_t size) STUB_ONLY

int sys_sleep(time_t *secs, long *nanos) STUB_ONLY
int sys_fork(pid_t *child) {
    pid_t ret;
    asm volatile ("syscall" : "=a"(ret)
            : "a"(10)
            : "rcx", "r11");
    *child = ret;
    return 0;
}
int sys_execve(const char *path, char *const argv[], char *const envp[]) STUB_ONLY
int sys_kill(int, int) STUB_ONLY
int sys_waitpid(pid_t pid, int *status, int flags) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return ENOSYS;
}
void sys_yield() STUB_ONLY

gid_t sys_getgid() {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
gid_t sys_getegid() {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
uid_t sys_getuid() {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
uid_t sys_geteuid() {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}
pid_t sys_getpid() {
    pid_t pid;
    asm volatile ("syscall" : "=a"(pid)
            : "a"(5)
            : "rcx", "r11");
    return pid;
}
pid_t sys_getppid() {
    mlibc::infoLogger() << "\e[31mmlibc: " << __func__ << " is a stub!" << frg::endlog;
    return 0;
}

#endif // MLIBC_BUILDING_RTDL

} // namespace mlibc

