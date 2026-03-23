#ifndef MLIBC_SYSDEP_TAGS
#define MLIBC_SYSDEP_TAGS

#include <abi-bits/mode_t.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/resource.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/signal.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/stat.h>
#include <abi-bits/uid_t.h>
#include <abi-bits/vm-flags.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>
#include <bits/cpu_set.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <concepts>
#include <mlibc-config.h>
#include <mlibc/fsfd_target.hpp>
#include <stddef.h>
#include <stdint.h>

// [[noreturn]] void sys_exit(int status);
struct Exit { static constexpr bool is_noreturn = true; };
// [[noreturn, gnu::weak]] void sys_thread_exit();
struct ThreadExit { static constexpr bool is_noreturn = true; };

// If *stack is not null, it should point to the lowest addressable byte of the stack.
// Returns the new stack pointer in *stack and the stack base in *stack_base.
// [[gnu::weak]] int sys_prepare_stack(void **stack, void *entry, void *user_arg, void* tcb, size_t *stack_size, size_t *guard_size, void **stack_base);
struct PrepareStack {};

// [[gnu::weak]] int sys_clone(void *tcb, pid_t *pid_out, void *stack);
struct Clone {};

// int sys_futex_wait(int *pointer, int expected, const struct timespec *time);
struct FutexWait {};
// int sys_futex_wake(int *pointer, bool all);
struct FutexWake {};

// int sys_open(const char *pathname, int flags, mode_t mode, int *fd);
struct Open {};
// [[gnu::weak]] int sys_flock(int fd, int options);
struct Flock {};

// [[gnu::weak]] int sys_open_dir(const char *path, int *handle);
struct OpenDir {};
// [[gnu::weak]] int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read);
struct ReadEntries {};

// int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
struct Read {};

// int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
struct Write {};
// [[gnu::weak]] int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read);
struct Pread {};

// int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
struct Seek {};
// int sysdep<Close>(int fd);
struct Close {};

// int sys_clock_get(int clock, time_t *secs, long *nanos);
struct ClockGet {};
// [[gnu::weak]] int sys_clock_set(int clock, time_t secs, long nanos);
struct ClockSet {};
// [[gnu::weak]] int sys_clock_getres(int clock, time_t *secs, long *nanos);
struct ClockGetres {};
// [[gnu::weak]] int sys_sleep(time_t *secs, long *nanos);
struct Sleep {};
// // In contrast to the isatty() library function, the sysdep function uses return value
// // zero (and not one) to indicate that the file is a terminal.
// [[gnu::weak]] int sys_isatty(int fd);
struct Isatty {};
// [[gnu::weak]] int sys_rmdir(const char *path);
struct Rmdir {};
// [[gnu::weak]] int sys_unlinkat(int dirfd, const char *path, int flags);
struct Unlinkat {};
// [[gnu::weak]] int sys_rename(const char *path, const char *new_path);
struct Rename {};

// [[gnu::weak]] int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
struct Sigprocmask {};
// [[gnu::weak]] int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
struct Sigaction {};

// [[gnu::weak]] int sys_fork(pid_t *child);
struct Fork {};
// [[gnu::weak]] int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid);
struct Waitpid {};
// [[gnu::weak]] int sys_execve(const char *path, char *const argv[], char *const envp[]);
struct Execve {};
// [[gnu::weak]] void sys_yield();
struct Yield {};

// [[gnu::weak]] pid_t sys_getpid();
struct GetPid {};
// [[gnu::weak]] int sys_kill(pid_t, int);
struct Kill {};

// int sys_tcb_set(void *pointer);
struct TcbSet {};

#if MLIBC_BUILDING_RTLD
// [[gnu::weak]] int sys_vm_readahead(void *pointer, size_t size);
struct VmReadahead {};
#endif /* MLIBC_BUILDING_RTLD */

// void sys_libc_log(const char *message);
struct LibcLog {};
// [[noreturn]] void sys_libc_panic();
struct LibcPanic { static constexpr bool is_noreturn = true; };

// [[gnu::weak]] int sys_futex_tid();
struct FutexTid {};

// int sys_anon_allocate(size_t size, void **pointer);
struct AnonAllocate {};
// int sys_anon_free(void *pointer, size_t size);
struct AnonFree {};

// [[gnu::weak]] int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf);
struct Stat {};

// int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);
struct VmMap {};
// int sys_vm_unmap(void *pointer, size_t size);
struct VmUnmap {};
// [[gnu::weak]] int sys_vm_protect(void *pointer, size_t size, int prot);
struct VmProtect {};

#if defined(__riscv)
#include <sys/hwprobe.h>

// [[gnu::weak]] int sys_riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags);
struct RiscvHwprobe {};
#endif

#if __MLIBC_BSD_OPTION
#include <termios.h>

// [[gnu::weak]] int sys_brk(void **out);
struct Brk {};
// [[gnu::weak]] int sys_getloadavg(double *samples);
struct GetLoadavg {};
// [[gnu::weak]] int sys_openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win);
struct Openpty {};
#endif // __MLIBC_BSD_OPTION

#if __MLIBC_GLIBC_OPTION
// [[gnu::weak]] int sys_personality(unsigned long persona, int *out);
struct Personality {};
// [[gnu::weak]] int sys_ioperm(unsigned long int from, unsigned long int num, int turn_on);
struct Ioperm {};
// [[gnu::weak]] int sys_iopl(int level);
struct Iopl {};

#ifdef __riscv
// [[gnu::weak]] int sys_riscv_flush_icache(void *start, void *end, unsigned long flags);
struct RiscvFlushIcache {};
#endif
#endif // __MLIBC_GLIBC_OPTION

#if __MLIBC_POSIX_OPTION
#include <abi-bits/msg.h>
#include <abi-bits/seek-whence.h>
#include <abi-bits/socklen_t.h>
#include <abi-bits/vm-flags.h>
#include <bits/off_t.h>
#include <bits/posix/stat.h>
#include <bits/ssize_t.h>
#include <fcntl.h>
#include <mlibc/fsfd_target.hpp>
#include <mqueue.h>
#include <poll.h>
#include <sched.h>
#include <stdarg.h>
#include <sys/poll.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <ucontext.h>

// [[gnu::weak]] int sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read);
struct Readv {};
// [[gnu::weak]] int sys_writev(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written);
struct Writev {};
// [[gnu::weak]] int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_read);
struct Pwrite {};
// [[gnu::weak]] int sys_access(const char *path, int mode);
struct Access {};
// [[gnu::weak]] int sys_faccessat(int dirfd, const char *pathname, int mode, int flags);
struct Faccessat {};
// [[gnu::weak]] int sys_dup(int fd, int flags, int *newfd);
struct Dup {};
// [[gnu::weak]] int sys_dup2(int fd, int flags, int newfd);
struct Dup2 {};
// [[gnu::weak]] int sys_statvfs(const char *path, struct statvfs *out);
struct Statvfs {};
// [[gnu::weak]] int sys_fstatvfs(int fd, struct statvfs *out);
struct Fstatvfs {};
// [[gnu::weak]] int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length);
struct Readlink {};
// [[gnu::weak]] int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length);
struct Readlinkat {};
// [[gnu::weak]] int sys_truncate(const char *path, off_t length);
struct Truncate {};
// [[gnu::weak]] int sys_ftruncate(int fd, size_t size);
struct Ftruncate {};
// [[gnu::weak]] int sys_fallocate(int fd, off_t offset, size_t size);
struct Fallocate {};
// [[gnu::weak]] int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd);
struct Openat {};
// [[gnu::weak]] int sys_socket(int family, int type, int protocol, int *fd);
struct Socket {};
// [[gnu::weak]] int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length);
struct MsgSend {};
// [[gnu::weak]] int sys_sendto(int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length);
struct Sendto {};
// [[gnu::weak]] int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length);
struct MsgRecv {};
// [[gnu::weak]] int sys_recvfrom(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length);
struct Recvfrom {};
// [[gnu::weak]] int sys_listen(int fd, int backlog);
struct Listen {};
// [[gnu::weak]] gid_t sys_getgid();
struct GetGid {};
// [[gnu::weak]] gid_t sys_getegid();
struct GetEgid {};
// [[gnu::weak]] uid_t sys_getuid();
struct GetUid {};
// [[gnu::weak]] uid_t sys_geteuid();
struct GetEuid {};
// [[gnu::weak]] pid_t sys_gettid();
struct GetTid {};
// [[gnu::weak]] pid_t sys_getppid();
struct GetPpid {};
// [[gnu::weak]] int sys_getpgid(pid_t pid, pid_t *pgid);
struct GetPgid {};
// [[gnu::weak]] int sys_getsid(pid_t pid, pid_t *sid);
struct GetSid {};
// [[gnu::weak]] int sys_setpgid(pid_t pid, pid_t pgid);
struct SetPgid {};
// [[gnu::weak]] int sys_setuid(uid_t uid);
struct SetUid {};
// [[gnu::weak]] int sys_seteuid(uid_t euid);
struct SetEuid {};
// [[gnu::weak]] int sys_setgid(gid_t gid);
struct SetGid {};
// [[gnu::weak]] int sys_setegid(gid_t egid);
struct SetEgid {};
// [[gnu::weak]] int sys_getgroups(size_t size, gid_t *list, int *ret);
struct GetGroups {};
// [[gnu::weak]] int sys_fexecve(int fd, char *const argv[], char *const envp[]);
struct Fexecve {};
// [[gnu::weak]] int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events);
struct Pselect {};
// [[gnu::weak]] int sys_getrusage(int scope, struct rusage *usage);
struct GetRusage {};
// [[gnu::weak]] int sys_getrlimit(int resource, struct rlimit *limit);
struct GetRlimit {};
// [[gnu::weak]] int sys_setrlimit(int resource, const struct rlimit *limit);
struct SetRlimit {};
// [[gnu::weak]] int sys_getpriority(int which, id_t who, int *value);
struct GetPriority {};
// [[gnu::weak]] int sys_setpriority(int which, id_t who, int prio);
struct SetPriority {};
// [[gnu::weak]] int sys_getschedparam(void *tcb, int *policy, struct sched_param *param);
struct GetSchedparam {};
// [[gnu::weak]] int sys_setschedparam(void *tcb, int policy, const struct sched_param *param);
struct SetSchedparam {};
// [[gnu::weak]] int sys_getscheduler(pid_t pid, int *policy);
struct GetScheduler {};
// [[gnu::weak]] int sys_setscheduler(pid_t pid, int policy, const struct sched_param *param);
struct SetScheduler {};
// [[gnu::weak]] int sys_getparam(pid_t pid, struct sched_param *param);
struct GetParam {};
// [[gnu::weak]] int sys_setparam(pid_t pid, const struct sched_param *param);
struct SetParam {};
// [[gnu::weak]] int sys_get_max_priority(int policy, int *out);
struct GetMaxPriority {};
// [[gnu::weak]] int sys_get_min_priority(int policy, int *out);
struct GetMinPriority {};
// [[gnu::weak]] int sys_getcwd(char *buffer, size_t size);
struct GetCwd {};
// [[gnu::weak]] int sys_chdir(const char *path);
struct Chdir {};
// [[gnu::weak]] int sys_fchdir(int fd);
struct Fchdir {};
// [[gnu::weak]] int sys_chroot(const char *path);
struct Chroot {};
// [[gnu::weak]] int sys_mkdir(const char *path, mode_t mode);
struct Mkdir {};
// [[gnu::weak]] int sys_mkdirat(int dirfd, const char *path, mode_t mode);
struct Mkdirat {};
// [[gnu::weak]] int sys_link(const char *old_path, const char *new_path);
struct Link {};
// [[gnu::weak]] int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags);
struct Linkat {};
// [[gnu::weak]] int sys_symlink(const char *target_path, const char *link_path);
struct Symlink {};
// [[gnu::weak]] int sys_symlinkat(const char *target_path, int dirfd, const char *link_path);
struct Symlinkat {};
// [[gnu::weak]] int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path);
struct Renameat {};
// [[gnu::weak]] int sys_fcntl(int fd, int request, va_list args, int *result);
struct Fcntl {};
// [[gnu::weak]] int sys_ttyname(int fd, char *buf, size_t size);
struct Ttyname {};
// [[gnu::weak]] int sys_fadvise(int fd, off_t offset, off_t length, int advice);
struct Fadvise {};
// [[gnu::weak]] void sys_sync();
struct Sync {};
// [[gnu::weak]] int sys_fsync(int fd);
struct Fsync {};
// [[gnu::weak]] int sys_fdatasync(int fd);
struct Fdatasync {};
// [[gnu::weak]] int sys_chmod(const char *pathname, mode_t mode);
struct Chmod {};
// [[gnu::weak]] int sys_fchmod(int fd, mode_t mode);
struct Fchmod {};
// [[gnu::weak]] int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags);
struct Fchmodat {};
// [[gnu::weak]] int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags);
struct Utimensat {};
// [[gnu::weak]] int sys_mlock(const void *addr, size_t length);
struct Mlock {};
// [[gnu::weak]] int sys_munlock(const void *addr, size_t length);
struct Munlock {};
// [[gnu::weak]] int sys_mlockall(int flags);
struct Mlockall {};
// [[gnu::weak]] int sys_munlockall(void);
struct Munlockall {};
// [[gnu::weak]] int sys_mincore(void *addr, size_t length, unsigned char *vec);
struct Mincore {};
// [[gnu::weak]] int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window);
struct VmRemap {};
// [[gnu::weak]] int sys_setsid(pid_t *sid);
struct SetSid {};
// [[gnu::weak]] int sys_tcgetattr(int fd, struct termios *attr);
struct Tcgetattr {};
// [[gnu::weak]] int sys_tcsetattr(int, int, const struct termios *attr);
struct Tcsetattr {};
// [[gnu::weak]] int sys_tcsendbreak(int fd, int dur);
struct Tcsendbreak {};
// [[gnu::weak]] int sys_tcflow(int, int);
struct Tcflow {};
// [[gnu::weak]] int sys_tcflush(int fd, int queue);
struct Tcflush {};
// [[gnu::weak]] int sys_tcdrain(int);
struct Tcdrain {};
// [[gnu::weak]] int sys_tcgetwinsize(int fd, struct winsize *winsz);
struct Tcgetwinsize {};
// [[gnu::weak]] int sys_tcsetwinsize(int fd, const struct winsize *winsz);
struct Tcsetwinsize {};
// [[gnu::weak]] int sys_pipe(int *fds, int flags);
struct Pipe {};
// [[gnu::weak]] int sys_socketpair(int domain, int type_and_flags, int proto, int *fds);
struct Socketpair {};
// [[gnu::weak]] int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events);
struct Poll {};
// [[gnu::weak]] int sys_ppoll(struct pollfd *fds, nfds_t count, const struct timespec *ts, const sigset_t *mask, int *num_events);
struct Ppoll {};
// [[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result);
struct Ioctl {};
// [[gnu::weak]] int sys_posix_devctl(int fd, int dcmd, void *__restrict dev_data_ptr, size_t nbyte, int *__restrict dev_info_ptr);
struct PosixDevctl {};
// [[gnu::weak]] int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size);
struct GetSockopt {};
// [[gnu::weak]] int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size);
struct SetSockopt {};
// [[gnu::weak]] int sys_shutdown(int sockfd, int how);
struct Shutdown {};
// [[gnu::weak]] int sys_sockatmark(int sockfd, int *out);
struct Sockatmark {};
// [[gnu::weak]] int sys_thread_sigmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
struct ThreadSigmask {};
// NOTE: POSIX says that behavior of timeout = nullptr is unspecified. We treat this case
// as an infinite timeout, making sigtimedwait(..., nullptr) equivalent to sigwaitinfo(...)
// [[gnu::weak]] int sys_sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal);
struct Sigtimedwait {};
// [[gnu::weak]] int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags);
struct Accept {};
// [[gnu::weak]] int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
struct Bind {};
// [[gnu::weak]] int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
struct Connect {};
// [[gnu::weak]] int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
struct Sockname {};
// [[gnu::weak]] int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
struct Peername {};
// [[gnu::weak]] int sys_gethostname(char *buffer, size_t bufsize);
struct GetHostname {};
// [[gnu::weak]] int sys_sethostname(const char *buffer, size_t bufsize);
struct SetHostname {};
// [[gnu::weak]] int sys_mkfifoat(int dirfd, const char *path, mode_t mode);
struct Mkfifoat {};
// [[gnu::weak]] int sys_getentropy(void *buffer, size_t length);
struct GetEntropy {};
// [[gnu::weak]] int sys_mknodat(int dirfd, const char *path, int mode, int dev);
struct Mknodat {};
// [[gnu::weak]] int sys_umask(mode_t mode, mode_t *old);
struct Umask {};
// [[gnu::weak]] int sys_before_cancellable_syscall(ucontext_t *uctx);
struct BeforeCancellableSyscall {};
// [[gnu::weak]] int sys_tgkill(int tgid, int tid, int sig);
struct Tgkill {};
// [[gnu::weak]] int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
struct Fchownat {};
// [[gnu::weak]] int sys_sigaltstack(const stack_t *ss, stack_t *oss);
struct Sigaltstack {};
// [[gnu::weak]] int sys_sigsuspend(const sigset_t *set);
struct Sigsuspend {};
// [[gnu::weak]] int sys_sigpending(sigset_t *set);
struct Sigpending {};
// [[gnu::weak]] int sys_sigqueue(pid_t pid, int sig, const union sigval val);
struct Sigqueue {};
// [[gnu::weak]] int sys_setgroups(size_t size, const gid_t *list);
struct SetGroups {};
// [[gnu::weak]] int sys_memfd_create(const char *name, int flags, int *fd);
struct MemfdCreate {};
// [[gnu::weak]] int sys_madvise(void *addr, size_t length, int advice);
struct Madvise {};
// [[gnu::weak]] int sys_posix_madvise(void *addr, size_t length, int advice);
struct PosixMadvise {};
// [[gnu::weak]] int sys_msync(void *addr, size_t length, int flags);
struct Msync {};
// [[gnu::weak]] int sys_getitimer(int which, struct itimerval *curr_value);
struct GetItimer {};
// [[gnu::weak]] int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
struct SetItimer {};
// [[gnu::weak]] int sys_timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res);
struct TimerCreate {};
// [[gnu::weak]] int sys_timer_settime(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old);
struct TimerSettime {};
// [[gnu::weak]] int sys_timer_gettime(timer_t t, struct itimerspec *val);
struct TimerGettime {};
// [[gnu::weak]] int sys_timer_delete(timer_t t);
struct TimerDelete {};
// [[gnu::weak]] int sys_timer_getoverrun(timer_t t, int *out);
struct TimerGetoverrun {};
// [[gnu::weak]] int sys_times(struct tms *tms, clock_t *out);
struct Times {};
// [[gnu::weak]] int sys_uname(struct utsname *buf);
struct Uname {};
// [[gnu::weak]] int sys_pause();
struct Pause {};
// [[gnu::weak]] int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid);
struct SetResuid {};
// [[gnu::weak]] int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
struct SetResgid {};
// [[gnu::weak]] int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
struct GetResuid {};
// [[gnu::weak]] int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
struct GetResgid {};
// [[gnu::weak]] int sys_setreuid(uid_t ruid, uid_t euid);
struct SetReuid {};
// [[gnu::weak]] int sys_setregid(gid_t rgid, gid_t egid);
struct SetRegid {};
// [[gnu::weak]] int sys_getlogin_r(char *name, size_t name_len);
struct GetLoginR {};
// [[gnu::weak]] int sys_if_indextoname(unsigned int index, char *name);
struct IfIndextoname {};
// [[gnu::weak]] int sys_if_nametoindex(const char *name, unsigned int *ret);
struct IfNametoindex {};
// [[gnu::weak]] int sys_ptsname(int fd, char *buffer, size_t length);
struct Ptsname {};
// [[gnu::weak]] int sys_unlockpt(int fd);
struct Unlockpt {};
// [[gnu::weak]] int sys_thread_setname(void *tcb, const char *name);
struct ThreadSetname {};
// [[gnu::weak]] int sys_thread_getname(void *tcb, char *name, size_t size);
struct ThreadGetname {};
// [[gnu::weak]] int sys_sysconf(int num, long *ret);
struct Sysconf {};
// [[gnu::weak]] int sys_semget(key_t key, int n, int fl, int *id);
struct Semget {};
// [[gnu::weak]] int sys_semctl(int semid, int semnum, int cmd, void *semun, int *ret);
struct Semctl {};
// [[gnu::weak]] int sys_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);
struct GetAffinity {};
// [[gnu::weak]] int sys_getthreadaffinity(pid_t tid, size_t cpusetsize, cpu_set_t *mask);
struct GetThreadaffinity {};
// [[gnu::weak]] int sys_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *mask);
struct SetAffinity {};
// [[gnu::weak]] int sys_setthreadaffinity(pid_t tid, size_t cpusetsize, const cpu_set_t *mask);
struct SetThreadaffinity {};
// [[gnu::weak]] int sys_get_current_stack_info(void **stack_base, size_t *stack_size);
struct GetCurrentStackInfo {};
// [[gnu::weak]] int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options);
struct Waitid {};
// [[gnu::weak]] int sys_name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags);
struct NameToHandleAt {};
// [[gnu::weak]] int sys_splice(int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags, ssize_t *out);
struct Splice {};
// [[gnu::weak]] int sys_shmat(void **seg_start, int shmid, const void *shmaddr, int shmflg);
struct Shmat {};
// [[gnu::weak]] int sys_shmctl(int *idx, int shmid, int cmd, struct shmid_ds *buf);
struct Shmctl {};
// [[gnu::weak]] int sys_shmdt(const void *shmaddr);
struct Shmdt {};
// [[gnu::weak]] int sys_shmget(int *shm_id, key_t key, size_t size, int shmflg);
struct Shmget {};
// [[gnu::weak]] int sys_inet_configured(bool *ipv4, bool *ipv6);
struct InetConfigured {};
// [[gnu::weak]] int sys_nice(int nice, int *new_nice);
struct Nice {};
// [[gnu::weak]] int sys_openpt(int oflags, int *fd);
struct Openpt {};
// [[gnu::weak]] int sys_msgctl(int q, int cmd, struct msqid_ds *buf);
struct Msgctl {};
// [[gnu::weak]] int sys_msgget(key_t k, int flag, int *out);
struct Msgget {};
// [[gnu::weak]] int sys_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out);
struct Msgrcv {};
// [[gnu::weak]] int sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
struct Msgsnd {};
// [[gnu::weak]] int sys_mq_open(const char *name, int oflag, mode_t mode, mq_attr *attr, mqd_t *out);
struct MqOpen {};
// [[gnu::weak]] int sys_mq_unlink(const char *name);
struct MqUnlink {};
// [[gnu::weak]] int sys_mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
struct MqReceive {};
// [[gnu::weak]] int sys_mq_get_attr(mqd_t mqdes, mq_attr *mqstat);
struct MqGetAttr {};
// [[gnu::weak]] int sys_mq_set_attr(mqd_t mqdes, mq_attr *mqstat, mq_attr *omqstat);
struct MqSetAttr {};
#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION
#include <ifaddrs.h>
#include <sched.h>
#include <stdarg.h>
#include <sys/sysinfo.h>
#include <sys/statfs.h>
#include <poll.h>
#include <linux/capability.h>
#include <abi-bits/pid_t.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/statx.h>
#include <bits/off_t.h>
#include <bits/ssize_t.h>
#include <bits/size_t.h>

// [[gnu::weak]] int sys_inotify_create(int flags, int *fd);
struct InotifyCreate {};
// [[gnu::weak]] int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd);
struct InotifyAddWatch {};
// [[gnu::weak]] int sys_inotify_rm_watch(int ifd, int wd);
struct InotifyRmWatch {};
// [[gnu::weak]] int sys_mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data);
struct Mount {};
// [[gnu::weak]] int sys_umount2(const char *target, int flags);
struct Umount2 {};
// [[gnu::weak]] int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out);
struct Ptrace {};
// [[gnu::weak]] int sys_capget(cap_user_header_t hdrp, cap_user_data_t datap);
struct Capget {};
// [[gnu::weak]] int sys_capset(cap_user_header_t hdrp, const cap_user_data_t datap);
struct Capset {};
// [[gnu::weak]] int sys_prctl(int option, va_list va, int *out);
struct Prctl {};
// [[gnu::weak]] int sys_init_module(void *module, unsigned long length, const char *args);
struct InitModule {};
// [[gnu::weak]] int sys_delete_module(const char *name, unsigned flags);
struct DeleteModule {};
// [[gnu::weak]] int sys_klogctl(int type, char *bufp, int len, int *out);
struct Klogctl {};
// [[gnu::weak]] int sys_getcpu(int *cpu);
struct Getcpu {};
// [[gnu::weak]] int sys_sysinfo(struct sysinfo *info);
struct Sysinfo {};
// [[gnu::weak]] int sys_swapon(const char *path, int flags);
struct Swapon {};
// [[gnu::weak]] int sys_swapoff(const char *path);
struct Swapoff {};
// [[gnu::weak]] int sys_setxattr(const char *path, const char *name, const void *val, size_t size, int flags);
struct Setxattr {};
// [[gnu::weak]] int sys_lsetxattr(const char *path, const char *name, const void *val, size_t size, int flags);
struct Lsetxattr {};
// [[gnu::weak]] int sys_fsetxattr(int fd, const char *name, const void *val, size_t size, int flags);
struct Fsetxattr {};
// [[gnu::weak]] int sys_getxattr(const char *path, const char *name, void *val, size_t size, ssize_t *nread);
struct Getxattr {};
// [[gnu::weak]] int sys_lgetxattr(const char *path, const char *name, void *val, size_t size, ssize_t *nread);
struct Lgetxattr {};
// [[gnu::weak]] int sys_fgetxattr(int fd, const char *name, void *val, size_t size, ssize_t *nread);
struct Fgetxattr {};
// [[gnu::weak]] int sys_listxattr(const char *path, char *list, size_t size, ssize_t *nread);
struct Listxattr {};
// [[gnu::weak]] int sys_llistxattr(const char *path, char *list, size_t size, ssize_t *nread);
struct Llistxattr {};
// [[gnu::weak]] int sys_flistxattr(int fd, char *list, size_t size, ssize_t *nread);
struct Flistxattr {};
// [[gnu::weak]] int sys_removexattr(const char *path, const char *name);
struct Removexattr {};
// [[gnu::weak]] int sys_lremovexattr(const char *path, const char *name);
struct Lremovexattr {};
// [[gnu::weak]] int sys_fremovexattr(int fd, const char *name);
struct Fremovexattr {};
// [[gnu::weak]] int sys_statfs(const char *path, struct statfs *buf);
struct Statfs {};
// [[gnu::weak]] int sys_fstatfs(int fd, struct statfs *buf);
struct Fstatfs {};
// [[gnu::weak]] int sys_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);
struct Statx {};
// [[gnu::weak]] int sys_getifaddrs(struct ifaddrs **);
struct Getifaddrs {};
// [[gnu::weak]] int sys_sendfile(int outfd, int infd, off_t *offset, size_t count, ssize_t *out);
struct Sendfile {};
// [[gnu::weak]] int sys_syncfs(int fd);
struct Syncfs {};
// [[gnu::weak]] int sys_unshare(int flags);
struct Unshare {};
// [[gnu::weak]] int sys_setns(int fd, int nstype);
struct SetNs {};
// [[gnu::weak]] int sys_pidfd_open(pid_t pid, unsigned int flags, int *outfd);
struct PidfdOpen {};
// [[gnu::weak]] int sys_pidfd_getpid(int fd, pid_t *outpid);
struct PidfdGetpid {};
// [[gnu::weak]] int sys_pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags);
struct PidfdSendSignal {};
// [[gnu::weak]] int sys_process_vm_readv(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
struct ProcessVmReadv {};
// [[gnu::weak]] int sys_process_vm_writev(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
struct ProcessVmWritev {};
// [[gnu::weak]] int sys_fsopen(const char *fsname, unsigned int flags, int *outfd);
struct Fsopen {};
// [[gnu::weak]] int sys_fsmount(int fsfd, unsigned int flags, unsigned int mountflags, int *outfd);
struct Fsmount {};
// [[gnu::weak]] int sys_fsconfig(int fd, unsigned int cmd, const char *key, const void *val, int aux);
struct Fsconfig {};
// [[gnu::weak]] int sys_move_mount(int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags);
struct MoveMount {};
// [[gnu::weak]] int sys_open_tree(int dirfd, const char *path, unsigned int flags, int *outfd);
struct OpenTree {};
// [[gnu::weak]] int sys_copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags, ssize_t *bytes_copied);
struct CopyFileRange {};
#endif // __MLIBC_LINUX_OPTION

#if __MLIBC_LINUX_EPOLL_OPTION
#include <sys/epoll.h>

// [[gnu::weak]] int sys_epoll_create(int flags, int *fd);
struct EpollCreate {};
// [[gnu::weak]] int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
struct EpollCtl {};
// [[gnu::weak]] int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised);
struct EpollPwait {};
#endif // __MLIBC_LINUX_EPOLL_OPTION

#if __MLIBC_LINUX_TIMERFD_OPTION
#include <sys/timerfd.h>

// [[gnu::weak]] int sys_timerfd_create(int clockid, int flags, int *fd);
struct TimerfdCreate {};
// [[gnu::weak]] int sys_timerfd_settime(int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue);
struct TimerfdSettime {};
// [[gnu::weak]] int sys_timerfd_gettime(int fd, struct itimerspec *its);
struct TimerfdGettime {};
#endif // __MLIBC_LINUX_TIMERFD_OPTION

#if __MLIBC_LINUX_SIGNALFD_OPTION
#include <sys/signalfd.h>

// [[gnu::weak]] int sys_signalfd_create(const sigset_t *, int flags, int *fd);
struct SignalfdCreate {};
#endif // __MLIBC_LINUX_SIGNALFD_OPTION

#if __MLIBC_LINUX_EVENTFD_OPTION
#include <sys/eventfd.h>

// [[gnu::weak]] int sys_eventfd_create(unsigned int initval, int flags, int *fd);
struct EventfdCreate {};
#endif // __MLIBC_LINUX_EVENTFD_OPTION

#if __MLIBC_LINUX_REBOOT_OPTION
#include <sys/reboot.h>

// [[gnu::weak]] int sys_reboot(int cmd);
struct Reboot {};
#endif // __MLIBC_LINUX_REBOOT_OPTION

#if __MLIBC_LINUX_WRAPPERS_OPTION
// [[gnu::weak]] int sys_rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo);
struct RtSigqueueinfo {};
// [[gnu::weak]] int sys_rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo);
struct RtTgSigqueueinfo {};
#endif // __MLIBC_LINUX_WRAPPERS_OPTION

#endif /* MLIBC_SYSDEP_TAGS */
