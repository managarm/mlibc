#include "syscall.h"
#include <asm/ioctls.h>
#include <bits/ensure.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <nyaux/syscalls.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#define STUB_ONLY                                                              \
  {                                                                            \
    __ensure(!"STUB_ONLY function was called");                                \
    __builtin_unreachable();                                                   \
  }

namespace mlibc {

void sys_libc_log(const char *message) {
  __syscall2(SYSCALL_DEBUG, (uint64_t)message, strlen(message));
}
void sys_exit(int status) {
  __syscall1(SYSCALL_EXIT, status);
  __builtin_unreachable();
};
void sys_libc_panic() {
  sys_libc_log("\nMLIBC PANIC\n");
  sys_exit(1);
}
int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd,
               off_t offset, void **window) {
  struct __syscall_ret ret =
      __syscall6(SYSCALL_MMAP, (uint64_t)hint, (uint64_t)size, (uint64_t)prot,
                 (uint64_t)flags, (uint64_t)fd, (uint64_t)offset);
  *window = (void *)ret.ret;
  return ret.err;
};
int sys_anon_allocate(size_t size, void **pointer) {
  return sys_vm_map(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0,
                    pointer);
};
int sys_vm_protect(void *pointer, size_t size, int prot) { return 0; }
int sys_anon_free(void *pointer, size_t size) {
  __syscall2(SYSCALL_FREE, (uint64_t)pointer, size);
  return 0;
};
int sys_close(int fd) {
  __syscall_ret ret = __syscall1(SYSCALL_CLOSE, fd);

  if (ret.err != 0)
    return ret.err;

  return 0;
};
int sys_futex_wait(int *pointer, int expected,
                   const struct timespec *time) STUB_ONLY;
int sys_futex_wake(int *pointer) {
  sys_libc_log("sys futex wake is a stub");
  return 0;
};
int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
  __syscall_ret ret =
      __syscall4(SYSCALL_OPENAT, dirfd, (uint64_t)path, flags, mode);

  if (ret.err != 0)
    return ret.err;

  *fd = (int)ret.ret;
  return 0;
}

uid_t sys_geteuid() {
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}
int sys_gethostname(char *buffer, size_t bufsize) {
  if (bufsize >= 6) {
    memcpy(buffer, "nyaux", 6);
  }
  return 0;
}

gid_t sys_getgid() {
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}

int sys_setgid(gid_t gid) {
  (void)gid;
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}

pid_t sys_getpgid(pid_t pid, pid_t *pgid) {
  (void)pid;
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  *pgid = 0;
  return 0;
}

gid_t sys_getegid() {
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
  (void)pid;
  (void)pgid;
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}

int sys_ttyname(int fd, char *buf, size_t size) {
  (void)fd;
  (void)buf;
  (void)size;
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return ENOSYS;
}
uid_t sys_getuid() {
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n"
                      << frg::endlog;
  return 0;
}
int sys_open(const char *path, int flags, mode_t mode, int *fd) {
  return sys_openat(AT_FDCWD, path, flags, mode, fd);
}
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
  __syscall_ret ret = __syscall3(SYSCALL_READ, fd, (uint64_t)buf, count);

  if (ret.err != 0) {
    return ret.err;
  }

  *bytes_read = (ssize_t)ret.ret;
  return 0;
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
  __syscall_ret ret = __syscall3(SYSCALL_SEEK, fd, offset, whence);

  if (ret.err != 0) {
    return ret.err;
  }

  *new_offset = (off_t)ret.ret;
  return 0;
}
int sys_tcb_set(void *pointer) {
  __syscall_ret ret = __syscall1(SYSCALL_SETFSBASE, (uint64_t)pointer);
  if (ret.err != 0) {
    return ret.err;
  }
  return 0;
};
int sys_isatty(int fd) {
  __syscall_ret ret = __syscall1(SYSCALL_ISATTY, fd);
  if (ret.err != 0) {
    return ret.err;
  }

  return ret.ret;
}
int sys_vm_unmap(void *pointer, size_t size) STUB_ONLY;
int sys_clock_get(int clock, time_t *secs, long *nanos) { return ENOSYS; }
#ifndef MLIBC_BUILDING_RTLD
int sys_getcwd(char *buffer, size_t size) {
  __syscall_ret ret = __syscall2(SYSCALL_GETCWD, (uint64_t)buffer, size);

  if (ret.err != 0)
    return ret.err;

  return 0;
}
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
  __syscall_ret ret = __syscall3(SYSCALL_WRITE, fd, (uint64_t)buf, count);

  if (ret.err != 0) {
    return ret.err;
  }

  *bytes_written = (ssize_t)ret.ret;

  return 0;
}

pid_t sys_getpid() {
  __syscall_ret ret = __syscall0(16);
  return ret.ret;
}
#endif
pid_t sys_getppid() {
  mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;

  return 0;
}
#ifndef MLIBC_BUILDING_RTLD
int sys_fork(pid_t *child) {
  __syscall_ret ret = __syscall0(SYSCALL_FORK);

  if (ret.err != 0) {
    return ret.err;
  }

  *child = (pid_t)ret.ret;
  return 0;
}
int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
  __syscall_ret ret = __syscall3(SYSCALL_IOCTL, fd, request, (uint64_t)arg);

  if (ret.err != 0)
    return ret.err;
  *result = (int)ret.ret;
  return 0;
}
int sys_fcntl(int fd, int request, va_list args, int *result) {
  // __syscall_ret ret = __syscall(12, fd, request, va_arg(args, uint64_t));

  // if (ret.errno != 0)
  // 	return ret.errno;

  // *result = (ssize_t)ret.ret;
  // return 0;
  mlibc::infoLogger() << "mlibc: fd " << fd << " request " << request
                      << frg::endlog;
  return ENOSYS;
}
int sys_pselect(int nfds, fd_set *read_set, fd_set *write_set,
                fd_set *except_set, const struct timespec *timeout,
                const sigset_t *sigmask, int *num_events) {
  // this is a stub to please bash
  fd_set res_read_set;
  fd_set res_write_set;
  fd_set res_except_set;
  FD_ZERO(&res_read_set);
  FD_ZERO(&res_write_set);
  FD_ZERO(&res_except_set);
  for (int i = 0; i < nfds; i++) {
    if (read_set && FD_ISSET(i, read_set)) {
      FD_SET(i, &res_read_set);
    }
    if (write_set && FD_ISSET(i, write_set)) {
      FD_SET(i, &res_write_set);
    }
    if (except_set && FD_ISSET(i, except_set)) {
      FD_SET(i, &res_except_set);
    }
  }
  if (read_set)
    *read_set = res_read_set;
  if (write_set)
    *write_set = res_write_set;
  if (except_set)
    *except_set = res_except_set;
  *num_events = 1;
  return 0;
}
int sys_tcsetattr(int fd, int optional_action, const struct termios *attr) {
  int ret;

  switch (optional_action) {
  case TCSANOW:
    optional_action = TCSETS;
    break;
  case TCSADRAIN:
    return ENOSYS;
    break;
  case TCSAFLUSH:
    return ENOSYS;
    break;
  default:
    __ensure(!"Unsupported tcsetattr");
  }

  if (int r = sys_ioctl(fd, optional_action, (void *)attr, &ret) != 0) {
    return r;
  }

  return 0;
}
int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru,
                pid_t *ret_pid) {
  if (ru) {
    mlibc::infoLogger() << "mlibc: struct rusage in sys_waitpid is unsupported"
                        << frg::endlog;
    return ENOSYS;
  }

again:
  __syscall_ret ret = __syscall3(SYSCALL_WAITPID, pid, (uint64_t)status, flags);

  if (ret.err != 0) {
    if (ret.err == EINTR) {
      goto again;
    }

    return ret.err;
  }

  *ret_pid = (pid_t)ret.ret;
  return 0;
}
int sys_execve(const char *path, char *const argv[], char *const envp[]) {
  __syscall_ret ret = __syscall3(SYSCALL_EXECVE, (uint64_t)path, (uint64_t)argv,
                                 (uint64_t)envp);
  if (ret.ret != 0) {
    return ret.err;
  } else {
    __builtin_unreachable();
  }
}
int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
  __syscall_ret ret =
      __syscall4(SYSCALL_FACCESSAT, dirfd, (uint64_t)pathname, mode, flags);

  if (ret.err != 0) {
    return ret.err;
  }

  return 0;
}

int sys_access(const char *path, int mode) {
  return sys_faccessat(AT_FDCWD, path, mode, 0);
}
#endif
int sys_sigprocmask(int how, const sigset_t *__restrict set,
                    sigset_t *__restrict retrieve) {
  return ENOSYS;
}
int sys_sigaction(int signum, const struct sigaction *act,
                  struct sigaction *oldact) {
  return ENOSYS;
}
int sys_tcgetattr(int fd, struct termios *attr) {
  int ret;

  if (int r = sys_ioctl(fd, TCGETS, attr, &ret) != 0) {
    return r;
  }

  return 0;
}
int sys_dup(int fd, int flags, int *newfd) {
  (void)flags;
  __syscall_ret ret = __syscall2(SYSCALL_DUP, fd, flags);

  if (ret.err != 0)
    return ret.err;

  *newfd = (ssize_t)ret.ret;
  return 0;
}
int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags,
             struct stat *statbuf) {
  __syscall_ret ret;
  switch (fsfdt) {
  case fsfd_target::fd: {
    ret = __syscall2(SYSCALL_FSTAT, fd, (uint64_t)statbuf);
    if (ret.err != 0) {
      return ret.err;
    }
    return ret.ret;
    break;
  }
  case fsfd_target::path: {
    int fd = 0;
    int bad = sys_openat(AT_FDCWD, path, flags, O_RDONLY, &fd);
    if (bad != 0) {
      return bad;
    }
    ret = __syscall2(SYSCALL_FSTAT, fd, (uint64_t)statbuf);
    bad = sys_close(fd);
    // ret = __syscall(11, AT_FDCWD, path, statbuf, flags);
    return ret.ret;
    break;
  }
  case fsfd_target::fd_path: {
    mlibc::infoLogger() << "mlibc: statfd_path is a stub" << frg::endlog;
    // ret = __syscall(11, fd, path, statbuf, flags);
    return ENOSYS;
    break;
  }
  default: {
    __ensure(!"stat: Invalid fsfdt");
    __builtin_unreachable();
  }
  }
  if (ret.err != 0)
    return ret.err;
  return ret.ret;
}

} // namespace mlibc
