#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <crcnt/syscall.h>
#include <errno.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc {

void sys_libc_log(const char *message) {
    // WARNING: Syscall log is going away soon in a later version of mrk
    syscall(SYS_LOG, message);
}

void sys_libc_panic() {
    sys_libc_log("\nMLIBC PANIC\n");
    // Generate a exception
    asm volatile ("int $3");
}

void sys_exit(int status) STUB_ONLY

int sys_tcb_set(void *pointer) {
    syscall(SYS_SET_FS_BASE, pointer);
    return 0;
}

int sys_futex_wait(int *pointer, int expected) STUB_ONLY
int sys_futex_wake(int *pointer) STUB_ONLY

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) STUB_ONLY

int sys_isatty(int fd) {
    return ENOTTY;
}

int sys_openat(int dirfd, const char *path, int flags, int *fd) STUB_ONLY

int sys_open(const char *path, int flags, int *fd) STUB_ONLY
int sys_open_dir(const char *path, int *handle) STUB_ONLY

int sys_read_entries(int fd, void *buffer, size_t max_size, size_t *bytes_read) STUB_ONLY

int sys_close(int fd) STUB_ONLY

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) STUB_ONLY
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) STUB_ONLY
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) STUB_ONLY
int sys_vm_map(void *hint, size_t size, int prot, int flags,
               int fd, off_t offset, void **window) STUB_ONLY
int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY

int sys_anon_allocate(size_t size, void **pointer) {
    int errno = sys_vm_map(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE,
                           MAP_ANONYMOUS, -1, 0, pointer);
    return errno;
}

int sys_anon_free(void *pointer, size_t size) {
    return sys_vm_unmap(pointer, size);
}

pid_t sys_getpid() STUB_ONLY
pid_t sys_getppid() STUB_ONLY

uid_t sys_getuid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return 0;
}

uid_t sys_geteuid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return 0;
}

gid_t sys_getgid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return 0;
}

gid_t sys_getegid() {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return ENOSYS;
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    *secs  = 0;
    *nanos = 0;
    return 0;
}

int sys_gethostname(char *buffer, size_t bufsize) {
    const char *hostname = "crecent";
    for (size_t i = 0; i < bufsize; i) {
        buffer[i] = hostname[i];
        if (hostname[i] == 0)
            break;
    }
    return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) STUB_ONLY
int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) STUB_ONLY
int sys_access(const char *path, int mode) STUB_ONLY
int sys_chdir(const char *path) STUB_ONLY
int sys_mkdir(const char *path) STUB_ONLY
int sys_mkdirat(int dirfd, const char *path, mode_t mode) STUB_ONLY

int sys_socket(int domain, int type_and_flags, int proto, int *fd) STUB_ONLY
int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) STUB_ONLY
int sys_fork(pid_t *child) STUB_ONLY

int sys_execve(const char *path, char *const argv[], char *const envp[]) STUB_ONLY
int sys_fcntl(int fd, int request, va_list args, int *result) STUB_ONLY
int sys_dup(int fd, int flags, int *newfd) STUB_ONLY
int sys_dup2(int fd, int flags, int newfd) STUB_ONLY

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return ENOSYS;
}

int sys_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
    return 0;
}

int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set,
		const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
    mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;
	return 0;
}

int sys_waitpid(pid_t pid, int *status, int flags, pid_t *ret_pid) STUB_ONLY
} // namespace mlibc
