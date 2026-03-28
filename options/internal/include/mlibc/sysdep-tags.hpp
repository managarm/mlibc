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
// [[noreturn]] void sys_thread_exit();
struct ThreadExit { static constexpr bool is_noreturn = true; };

// If *stack is not null, it should point to the lowest addressable byte of the stack.
// Returns the new stack pointer in *stack and the stack base in *stack_base.
// int sys_prepare_stack(void **stack, void *entry, void *user_arg, void* tcb, size_t *stack_size, size_t *guard_size, void **stack_base);
struct PrepareStack {};

// int sys_clone(void *tcb, pid_t *pid_out, void *stack);
struct Clone {};

// int sys_futex_wait(int *pointer, int expected, const struct timespec *time);
struct FutexWait {};
// int sys_futex_wake(int *pointer, bool all);
struct FutexWake {};

// int sys_open(const char *pathname, int flags, mode_t mode, int *fd);
struct Open {};
// int sys_flock(int fd, int options);
struct Flock {};

// int sys_open_dir(const char *path, int *handle);
struct OpenDir {};
// int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read);
struct ReadEntries {};

// int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
struct Read {};

// int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
struct Write {};
// int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read);
struct Pread {};

// int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
struct Seek {};
// int sysdep<Close>(int fd);
struct Close {};

// int sys_clock_get(int clock, time_t *secs, long *nanos);
struct ClockGet {};
// int sys_clock_set(int clock, time_t secs, long nanos);
struct ClockSet {};
// int sys_clock_getres(int clock, time_t *secs, long *nanos);
struct ClockGetres {};
// int sys_sleep(time_t *secs, long *nanos);
struct Sleep {};
// // In contrast to the isatty() library function, the sysdep function uses return value
// // zero (and not one) to indicate that the file is a terminal.
// int sys_isatty(int fd);
struct Isatty {};
// int sys_rmdir(const char *path);
struct Rmdir {};
// int sys_unlinkat(int dirfd, const char *path, int flags);
struct Unlinkat {};
// int sys_rename(const char *path, const char *new_path);
struct Rename {};
// int sys_fd_to_path(int fd, char **path);
struct FdToPath {};

// int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
struct Sigprocmask {};
// int sys_sigaction(int, const struct sigaction *__restrict, struct sigaction *__restrict);
struct Sigaction {};

// int sys_fork(pid_t *child);
struct Fork {};
// int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid);
struct Waitpid {};
// int sys_execve(const char *path, char *const argv[], char *const envp[]);
struct Execve {};
// void sys_yield();
struct Yield {};

// pid_t sys_getpid();
struct GetPid {};
// int sys_kill(pid_t, int);
struct Kill {};

// int sys_tcb_set(void *pointer);
struct TcbSet {};

#if MLIBC_BUILDING_RTLD
// int sys_vm_readahead(void *pointer, size_t size);
struct VmReadahead {};
#endif /* MLIBC_BUILDING_RTLD */

// void sys_libc_log(const char *message);
struct LibcLog {};
// [[noreturn]] void sys_libc_panic();
struct LibcPanic { static constexpr bool is_noreturn = true; };

// int sys_futex_tid();
struct FutexTid {};

// int sys_anon_allocate(size_t size, void **pointer);
struct AnonAllocate {};
// int sys_anon_free(void *pointer, size_t size);
struct AnonFree {};

// int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf);
struct Stat {};

// int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);
struct VmMap {};
// int sys_vm_unmap(void *pointer, size_t size);
struct VmUnmap {};
// int sys_vm_protect(void *pointer, size_t size, int prot);
struct VmProtect {};

#if defined(__riscv)
#include <sys/hwprobe.h>

// int sys_riscv_hwprobe(struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags);
struct RiscvHwprobe {};
#endif

#if __MLIBC_BSD_OPTION
#include <termios.h>

// int sys_brk(void **out);
struct Brk {};
// int sys_getloadavg(double *samples);
struct GetLoadavg {};
// int sys_openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win);
struct Openpty {};
#endif // __MLIBC_BSD_OPTION

#if __MLIBC_GLIBC_OPTION
// int sys_personality(unsigned long persona, int *out);
struct Personality {};
// int sys_ioperm(unsigned long int from, unsigned long int num, int turn_on);
struct Ioperm {};
// int sys_iopl(int level);
struct Iopl {};

#ifdef __riscv
// int sys_riscv_flush_icache(void *start, void *end, unsigned long flags);
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

// int sys_readv(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read);
struct Readv {};
// int sys_writev(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written);
struct Writev {};
// int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_read);
struct Pwrite {};
// int sys_access(const char *path, int mode);
struct Access {};
// int sys_faccessat(int dirfd, const char *pathname, int mode, int flags);
struct Faccessat {};
// int sys_dup(int fd, int flags, int *newfd);
struct Dup {};
// int sys_dup2(int fd, int flags, int newfd);
struct Dup2 {};
// int sys_statvfs(const char *path, struct statvfs *out);
struct Statvfs {};
// int sys_fstatvfs(int fd, struct statvfs *out);
struct Fstatvfs {};
// int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length);
struct Readlink {};
// int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length);
struct Readlinkat {};
// int sys_truncate(const char *path, off_t length);
struct Truncate {};
// int sys_ftruncate(int fd, size_t size);
struct Ftruncate {};
// int sys_fallocate(int fd, off_t offset, size_t size);
struct Fallocate {};
// int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd);
struct Openat {};
// int sys_socket(int family, int type, int protocol, int *fd);
struct Socket {};
// int sys_msg_send(int fd, const struct msghdr *hdr, int flags, ssize_t *length);
struct MsgSend {};
// int sys_sendto(int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length);
struct Sendto {};
// int sys_msg_recv(int fd, struct msghdr *hdr, int flags, ssize_t *length);
struct MsgRecv {};
// int sys_recvfrom(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length);
struct Recvfrom {};
// int sys_listen(int fd, int backlog);
struct Listen {};
// gid_t sys_getgid();
struct GetGid {};
// gid_t sys_getegid();
struct GetEgid {};
// uid_t sys_getuid();
struct GetUid {};
// uid_t sys_geteuid();
struct GetEuid {};
// pid_t sys_gettid();
struct GetTid {};
// pid_t sys_getppid();
struct GetPpid {};
// int sys_getpgid(pid_t pid, pid_t *pgid);
struct GetPgid {};
// int sys_getsid(pid_t pid, pid_t *sid);
struct GetSid {};
// int sys_setpgid(pid_t pid, pid_t pgid);
struct SetPgid {};
// int sys_setuid(uid_t uid);
struct SetUid {};
// int sys_seteuid(uid_t euid);
struct SetEuid {};
// int sys_setgid(gid_t gid);
struct SetGid {};
// int sys_setegid(gid_t egid);
struct SetEgid {};
// int sys_getgroups(size_t size, gid_t *list, int *ret);
struct GetGroups {};
// int sys_fexecve(int fd, char *const argv[], char *const envp[]);
struct Fexecve {};
// int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events);
struct Pselect {};
// int sys_getrusage(int scope, struct rusage *usage);
struct GetRusage {};
// int sys_getrlimit(int resource, struct rlimit *limit);
struct GetRlimit {};
// int sys_setrlimit(int resource, const struct rlimit *limit);
struct SetRlimit {};
// int sys_getpriority(int which, id_t who, int *value);
struct GetPriority {};
// int sys_setpriority(int which, id_t who, int prio);
struct SetPriority {};
// int sys_getschedparam(void *tcb, int *policy, struct sched_param *param);
struct GetSchedparam {};
// int sys_setschedparam(void *tcb, int policy, const struct sched_param *param);
struct SetSchedparam {};
// int sys_getscheduler(pid_t pid, int *policy);
struct GetScheduler {};
// int sys_setscheduler(pid_t pid, int policy, const struct sched_param *param);
struct SetScheduler {};
// int sys_getparam(pid_t pid, struct sched_param *param);
struct GetParam {};
// int sys_setparam(pid_t pid, const struct sched_param *param);
struct SetParam {};
// int sys_get_max_priority(int policy, int *out);
struct GetMaxPriority {};
// int sys_get_min_priority(int policy, int *out);
struct GetMinPriority {};
// int sys_getcwd(char *buffer, size_t size);
struct GetCwd {};
// int sys_chdir(const char *path);
struct Chdir {};
// int sys_fchdir(int fd);
struct Fchdir {};
// int sys_chroot(const char *path);
struct Chroot {};
// int sys_mkdir(const char *path, mode_t mode);
struct Mkdir {};
// int sys_mkdirat(int dirfd, const char *path, mode_t mode);
struct Mkdirat {};
// int sys_link(const char *old_path, const char *new_path);
struct Link {};
// int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags);
struct Linkat {};
// int sys_symlink(const char *target_path, const char *link_path);
struct Symlink {};
// int sys_symlinkat(const char *target_path, int dirfd, const char *link_path);
struct Symlinkat {};
// int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path);
struct Renameat {};
// int sys_fcntl(int fd, int request, va_list args, int *result);
struct Fcntl {};
// int sys_ttyname(int fd, char *buf, size_t size);
struct Ttyname {};
// int sys_fadvise(int fd, off_t offset, off_t length, int advice);
struct Fadvise {};
// void sys_sync();
struct Sync {};
// int sys_fsync(int fd);
struct Fsync {};
// int sys_fdatasync(int fd);
struct Fdatasync {};
// int sys_chmod(const char *pathname, mode_t mode);
struct Chmod {};
// int sys_fchmod(int fd, mode_t mode);
struct Fchmod {};
// int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags);
struct Fchmodat {};
// int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags);
struct Utimensat {};
// int sys_mlock(const void *addr, size_t length);
struct Mlock {};
// int sys_munlock(const void *addr, size_t length);
struct Munlock {};
// int sys_mlockall(int flags);
struct Mlockall {};
// int sys_munlockall(void);
struct Munlockall {};
// int sys_mincore(void *addr, size_t length, unsigned char *vec);
struct Mincore {};
// int sys_vm_remap(void *pointer, size_t size, size_t new_size, void **window);
struct VmRemap {};
// int sys_setsid(pid_t *sid);
struct SetSid {};
// int sys_tcgetattr(int fd, struct termios *attr);
struct Tcgetattr {};
// int sys_tcsetattr(int, int, const struct termios *attr);
struct Tcsetattr {};
// int sys_tcsendbreak(int fd, int dur);
struct Tcsendbreak {};
// int sys_tcflow(int, int);
struct Tcflow {};
// int sys_tcflush(int fd, int queue);
struct Tcflush {};
// int sys_tcdrain(int);
struct Tcdrain {};
// int sys_tcgetwinsize(int fd, struct winsize *winsz);
struct Tcgetwinsize {};
// int sys_tcsetwinsize(int fd, const struct winsize *winsz);
struct Tcsetwinsize {};
// int sys_pipe(int *fds, int flags);
struct Pipe {};
// int sys_socketpair(int domain, int type_and_flags, int proto, int *fds);
struct Socketpair {};
// int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events);
struct Poll {};
// int sys_ppoll(struct pollfd *fds, nfds_t count, const struct timespec *ts, const sigset_t *mask, int *num_events);
struct Ppoll {};
// int sys_ioctl(int fd, unsigned long request, void *arg, int *result);
struct Ioctl {};
// int sys_posix_devctl(int fd, int dcmd, void *__restrict dev_data_ptr, size_t nbyte, int *__restrict dev_info_ptr);
struct PosixDevctl {};
// int sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size);
struct GetSockopt {};
// int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size);
struct SetSockopt {};
// int sys_shutdown(int sockfd, int how);
struct Shutdown {};
// int sys_sockatmark(int sockfd, int *out);
struct Sockatmark {};
// int sys_thread_sigmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
struct ThreadSigmask {};
// NOTE: POSIX says that behavior of timeout = nullptr is unspecified. We treat this case
// as an infinite timeout, making sigtimedwait(..., nullptr) equivalent to sigwaitinfo(...)
// int sys_sigtimedwait(const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal);
struct Sigtimedwait {};
// int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags);
struct Accept {};
// int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
struct Bind {};
// int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
struct Connect {};
// int sys_sockname(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
struct Sockname {};
// int sys_peername(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
struct Peername {};
// int sys_gethostname(char *buffer, size_t bufsize);
struct GetHostname {};
// int sys_sethostname(const char *buffer, size_t bufsize);
struct SetHostname {};
// int sys_mkfifoat(int dirfd, const char *path, mode_t mode);
struct Mkfifoat {};
// int sys_getentropy(void *buffer, size_t length);
struct GetEntropy {};
// int sys_mknodat(int dirfd, const char *path, int mode, int dev);
struct Mknodat {};
// int sys_umask(mode_t mode, mode_t *old);
struct Umask {};
// int sys_before_cancellable_syscall(ucontext_t *uctx);
struct BeforeCancellableSyscall {};
// int sys_tgkill(int tgid, int tid, int sig);
struct Tgkill {};
// int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
struct Fchownat {};
// int sys_sigaltstack(const stack_t *ss, stack_t *oss);
struct Sigaltstack {};
// int sys_sigsuspend(const sigset_t *set);
struct Sigsuspend {};
// int sys_sigpending(sigset_t *set);
struct Sigpending {};
// int sys_sigqueue(pid_t pid, int sig, const union sigval val);
struct Sigqueue {};
// int sys_setgroups(size_t size, const gid_t *list);
struct SetGroups {};
// int sys_memfd_create(const char *name, int flags, int *fd);
struct MemfdCreate {};
// int sys_madvise(void *addr, size_t length, int advice);
struct Madvise {};
// int sys_posix_madvise(void *addr, size_t length, int advice);
struct PosixMadvise {};
// int sys_msync(void *addr, size_t length, int flags);
struct Msync {};
// int sys_getitimer(int which, struct itimerval *curr_value);
struct GetItimer {};
// int sys_setitimer(int which, const struct itimerval *new_value, struct itimerval *old_value);
struct SetItimer {};
// int sys_timer_create(clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res);
struct TimerCreate {};
// int sys_timer_settime(timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old);
struct TimerSettime {};
// int sys_timer_gettime(timer_t t, struct itimerspec *val);
struct TimerGettime {};
// int sys_timer_delete(timer_t t);
struct TimerDelete {};
// int sys_timer_getoverrun(timer_t t, int *out);
struct TimerGetoverrun {};
// int sys_times(struct tms *tms, clock_t *out);
struct Times {};
// int sys_uname(struct utsname *buf);
struct Uname {};
// int sys_pause();
struct Pause {};
// int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid);
struct SetResuid {};
// int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid);
struct SetResgid {};
// int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
struct GetResuid {};
// int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
struct GetResgid {};
// int sys_setreuid(uid_t ruid, uid_t euid);
struct SetReuid {};
// int sys_setregid(gid_t rgid, gid_t egid);
struct SetRegid {};
// int sys_getlogin_r(char *name, size_t name_len);
struct GetLoginR {};
// int sys_if_indextoname(unsigned int index, char *name);
struct IfIndextoname {};
// int sys_if_nametoindex(const char *name, unsigned int *ret);
struct IfNametoindex {};
// int sys_ptsname(int fd, char *buffer, size_t length);
struct Ptsname {};
// int sys_unlockpt(int fd);
struct Unlockpt {};
// int sys_thread_setname(void *tcb, const char *name);
struct ThreadSetname {};
// int sys_thread_getname(void *tcb, char *name, size_t size);
struct ThreadGetname {};
// int sys_sysconf(int num, long *ret);
struct Sysconf {};
// int sys_semget(key_t key, int n, int fl, int *id);
struct Semget {};
// int sys_semctl(int semid, int semnum, int cmd, void *semun, int *ret);
struct Semctl {};
// int sys_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask);
struct GetAffinity {};
// int sys_getthreadaffinity(pid_t tid, size_t cpusetsize, cpu_set_t *mask);
struct GetThreadaffinity {};
// int sys_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *mask);
struct SetAffinity {};
// int sys_setthreadaffinity(pid_t tid, size_t cpusetsize, const cpu_set_t *mask);
struct SetThreadaffinity {};
// int sys_get_current_stack_info(void **stack_base, size_t *stack_size);
struct GetCurrentStackInfo {};
// int sys_waitid(idtype_t idtype, id_t id, siginfo_t *info, int options);
struct Waitid {};
// int sys_name_to_handle_at(int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags);
struct NameToHandleAt {};
// int sys_splice(int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags, ssize_t *out);
struct Splice {};
// int sys_shmat(void **seg_start, int shmid, const void *shmaddr, int shmflg);
struct Shmat {};
// int sys_shmctl(int *idx, int shmid, int cmd, struct shmid_ds *buf);
struct Shmctl {};
// int sys_shmdt(const void *shmaddr);
struct Shmdt {};
// int sys_shmget(int *shm_id, key_t key, size_t size, int shmflg);
struct Shmget {};
// int sys_inet_configured(bool *ipv4, bool *ipv6);
struct InetConfigured {};
// int sys_nice(int nice, int *new_nice);
struct Nice {};
// int sys_openpt(int oflags, int *fd);
struct Openpt {};
// int sys_msgctl(int q, int cmd, struct msqid_ds *buf);
struct Msgctl {};
// int sys_msgget(key_t k, int flag, int *out);
struct Msgget {};
// int sys_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out);
struct Msgrcv {};
// int sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
struct Msgsnd {};
// int sys_mq_open(const char *name, int oflag, mode_t mode, mq_attr *attr, mqd_t *out);
struct MqOpen {};
// int sys_mq_unlink(const char *name);
struct MqUnlink {};
// int sys_mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
struct MqReceive {};
// int sys_mq_get_attr(mqd_t mqdes, mq_attr *mqstat);
struct MqGetAttr {};
// int sys_mq_set_attr(mqd_t mqdes, mq_attr *mqstat, mq_attr *omqstat);
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

// int sys_inotify_create(int flags, int *fd);
struct InotifyCreate {};
// int sys_inotify_add_watch(int ifd, const char *path, uint32_t mask, int *wd);
struct InotifyAddWatch {};
// int sys_inotify_rm_watch(int ifd, int wd);
struct InotifyRmWatch {};
// int sys_mount(const char *source, const char *target, const char *fstype, unsigned long flags, const void *data);
struct Mount {};
// int sys_umount2(const char *target, int flags);
struct Umount2 {};
// int sys_ptrace(long req, pid_t pid, void *addr, void *data, long *out);
struct Ptrace {};
// int sys_capget(cap_user_header_t hdrp, cap_user_data_t datap);
struct Capget {};
// int sys_capset(cap_user_header_t hdrp, const cap_user_data_t datap);
struct Capset {};
// int sys_prctl(int option, va_list va, int *out);
struct Prctl {};
// int sys_init_module(void *module, unsigned long length, const char *args);
struct InitModule {};
// int sys_delete_module(const char *name, unsigned flags);
struct DeleteModule {};
// int sys_klogctl(int type, char *bufp, int len, int *out);
struct Klogctl {};
// int sys_getcpu(int *cpu);
struct Getcpu {};
// int sys_sysinfo(struct sysinfo *info);
struct Sysinfo {};
// int sys_swapon(const char *path, int flags);
struct Swapon {};
// int sys_swapoff(const char *path);
struct Swapoff {};
// int sys_setxattr(const char *path, const char *name, const void *val, size_t size, int flags);
struct Setxattr {};
// int sys_lsetxattr(const char *path, const char *name, const void *val, size_t size, int flags);
struct Lsetxattr {};
// int sys_fsetxattr(int fd, const char *name, const void *val, size_t size, int flags);
struct Fsetxattr {};
// int sys_getxattr(const char *path, const char *name, void *val, size_t size, ssize_t *nread);
struct Getxattr {};
// int sys_lgetxattr(const char *path, const char *name, void *val, size_t size, ssize_t *nread);
struct Lgetxattr {};
// int sys_fgetxattr(int fd, const char *name, void *val, size_t size, ssize_t *nread);
struct Fgetxattr {};
// int sys_listxattr(const char *path, char *list, size_t size, ssize_t *nread);
struct Listxattr {};
// int sys_llistxattr(const char *path, char *list, size_t size, ssize_t *nread);
struct Llistxattr {};
// int sys_flistxattr(int fd, char *list, size_t size, ssize_t *nread);
struct Flistxattr {};
// int sys_removexattr(const char *path, const char *name);
struct Removexattr {};
// int sys_lremovexattr(const char *path, const char *name);
struct Lremovexattr {};
// int sys_fremovexattr(int fd, const char *name);
struct Fremovexattr {};
// int sys_statfs(const char *path, struct statfs *buf);
struct Statfs {};
// int sys_fstatfs(int fd, struct statfs *buf);
struct Fstatfs {};
// int sys_statx(int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);
struct Statx {};
// int sys_getifaddrs(struct ifaddrs **);
struct Getifaddrs {};
// int sys_sendfile(int outfd, int infd, off_t *offset, size_t count, ssize_t *out);
struct Sendfile {};
// int sys_syncfs(int fd);
struct Syncfs {};
// int sys_unshare(int flags);
struct Unshare {};
// int sys_setns(int fd, int nstype);
struct SetNs {};
// int sys_pidfd_open(pid_t pid, unsigned int flags, int *outfd);
struct PidfdOpen {};
// int sys_pidfd_getpid(int fd, pid_t *outpid);
struct PidfdGetpid {};
// int sys_pidfd_send_signal(int pidfd, int sig, siginfo_t *info, unsigned int flags);
struct PidfdSendSignal {};
// int sys_process_vm_readv(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
struct ProcessVmReadv {};
// int sys_process_vm_writev(pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
struct ProcessVmWritev {};
// int sys_fsopen(const char *fsname, unsigned int flags, int *outfd);
struct Fsopen {};
// int sys_fsmount(int fsfd, unsigned int flags, unsigned int mountflags, int *outfd);
struct Fsmount {};
// int sys_fsconfig(int fd, unsigned int cmd, const char *key, const void *val, int aux);
struct Fsconfig {};
// int sys_move_mount(int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags);
struct MoveMount {};
// int sys_open_tree(int dirfd, const char *path, unsigned int flags, int *outfd);
struct OpenTree {};
// int sys_copy_file_range(int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags, ssize_t *bytes_copied);
struct CopyFileRange {};
#endif // __MLIBC_LINUX_OPTION

#if __MLIBC_LINUX_EPOLL_OPTION
#include <sys/epoll.h>

// int sys_epoll_create(int flags, int *fd);
struct EpollCreate {};
// int sys_epoll_ctl(int epfd, int mode, int fd, struct epoll_event *ev);
struct EpollCtl {};
// int sys_epoll_pwait(int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised);
struct EpollPwait {};
#endif // __MLIBC_LINUX_EPOLL_OPTION

#if __MLIBC_LINUX_TIMERFD_OPTION
#include <sys/timerfd.h>

// int sys_timerfd_create(int clockid, int flags, int *fd);
struct TimerfdCreate {};
// int sys_timerfd_settime(int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue);
struct TimerfdSettime {};
// int sys_timerfd_gettime(int fd, struct itimerspec *its);
struct TimerfdGettime {};
#endif // __MLIBC_LINUX_TIMERFD_OPTION

#if __MLIBC_LINUX_SIGNALFD_OPTION
#include <sys/signalfd.h>

// int sys_signalfd_create(const sigset_t *, int flags, int *fd);
struct SignalfdCreate {};
#endif // __MLIBC_LINUX_SIGNALFD_OPTION

#if __MLIBC_LINUX_EVENTFD_OPTION
#include <sys/eventfd.h>

// int sys_eventfd_create(unsigned int initval, int flags, int *fd);
struct EventfdCreate {};
#endif // __MLIBC_LINUX_EVENTFD_OPTION

#if __MLIBC_LINUX_REBOOT_OPTION
#include <sys/reboot.h>

// int sys_reboot(int cmd);
struct Reboot {};
#endif // __MLIBC_LINUX_REBOOT_OPTION

#if __MLIBC_LINUX_WRAPPERS_OPTION
// int sys_rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo);
struct RtSigqueueinfo {};
// int sys_rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo);
struct RtTgSigqueueinfo {};
#endif // __MLIBC_LINUX_WRAPPERS_OPTION

#endif /* MLIBC_SYSDEP_TAGS */
