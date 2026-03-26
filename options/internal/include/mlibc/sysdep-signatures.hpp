#ifndef MLIBC_SYSDEP_SIGNATURES
#define MLIBC_SYSDEP_SIGNATURES

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

#include <mlibc/sysdep-tags.hpp>

namespace mlibc {

template<typename Tag>
struct SysdepImpl;

struct NoImpl {};

template<typename ST, typename Tag>
using SysdepOf = std::conditional_t<
    std::derived_from<ST, Tag>,
    mlibc::SysdepImpl<Tag>,
    NoImpl
>;

#define SYSDEP_FUNC(name, ...) template <> struct SysdepImpl<name> { static int operator()(__VA_ARGS__); }
#define SYSDEP_FUNC_RET(rettype, name, ...) template <> struct SysdepImpl<name> { static rettype operator()(__VA_ARGS__); }
#define SYSDEP_FUNC_NORETURN(name, ...) template <> struct SysdepImpl<name> { [[noreturn]] static void operator()(__VA_ARGS__); }

SYSDEP_FUNC_RET(void, LibcLog, const char *message);
SYSDEP_FUNC_NORETURN(LibcPanic);

SYSDEP_FUNC_RET(pid_t, FutexTid);
SYSDEP_FUNC(AnonAllocate, size_t size, void **pointer);
SYSDEP_FUNC(AnonFree, void *pointer, size_t size);
SYSDEP_FUNC(Stat, mlibc::fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf);
SYSDEP_FUNC(VmMap, void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window);
SYSDEP_FUNC(VmUnmap, void *pointer, size_t size);
SYSDEP_FUNC(VmProtect, void *pointer, size_t size, int prot);
SYSDEP_FUNC(TcbSet, void *pointer);
#if defined(__riscv)
SYSDEP_FUNC(RiscvHwprobe, struct riscv_hwprobe *pairs, size_t pair_count, size_t cpusetsize, cpu_set_t *cpus, unsigned int flags);
#endif

// ANSI option sysdeps
SYSDEP_FUNC_NORETURN(Exit, int status);
SYSDEP_FUNC_NORETURN(ThreadExit);
SYSDEP_FUNC(PrepareStack, void **stack, void *entry, void *user_arg, void* tcb, size_t *stack_size, size_t *guard_size, void **stack_base);
SYSDEP_FUNC(Clone, void *tcb, pid_t *pid_out, void *stack);
SYSDEP_FUNC(FutexWait, int *pointer, int expected, const struct timespec *time);
SYSDEP_FUNC(FutexWake, int *pointer, bool all);
SYSDEP_FUNC(Open, const char *pathname, int flags, mode_t mode, int *fd);
SYSDEP_FUNC(Flock, int fd, int options);
SYSDEP_FUNC(OpenDir, const char *path, int *handle);
SYSDEP_FUNC(ReadEntries, int handle, void *buffer, size_t max_size, size_t *bytes_read);
SYSDEP_FUNC(Read, int fd, void *buf, size_t count, ssize_t *bytes_read);
SYSDEP_FUNC(Write, int fd, const void *buf, size_t count, ssize_t *bytes_written);
SYSDEP_FUNC(Pread, int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read);
SYSDEP_FUNC(Seek, int fd, off_t offset, int whence, off_t *new_offset);
SYSDEP_FUNC(Close, int fd);
SYSDEP_FUNC(ClockGet, int clock, time_t *secs, long *nanos);
SYSDEP_FUNC(ClockSet, int clock, time_t secs, long nanos);
SYSDEP_FUNC(ClockGetres, int clock, time_t *secs, long *nanos);
SYSDEP_FUNC(Sleep, time_t *secs, long *nanos);
SYSDEP_FUNC(Isatty, int fd);
SYSDEP_FUNC(Rmdir, const char *path);
SYSDEP_FUNC(Unlinkat, int dirfd, const char *path, int flags);
SYSDEP_FUNC(Rename, const char *path, const char *new_path);
SYSDEP_FUNC(FdToPath, int fd, char **out);
SYSDEP_FUNC(Sigprocmask, int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
SYSDEP_FUNC(Sigaction, int, const struct sigaction *__restrict, struct sigaction *__restrict);
SYSDEP_FUNC(Fork, pid_t *child);
SYSDEP_FUNC(Waitpid, pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid);
SYSDEP_FUNC(Execve, const char *path, char *const argv[], char *const envp[]);
SYSDEP_FUNC_RET(void, Yield);
SYSDEP_FUNC_RET(pid_t, GetPid);
SYSDEP_FUNC(Kill, pid_t, int);

#if MLIBC_BUILDING_RTLD
SYSDEP_FUNC(VmReadahead, void *pointer, size_t size);
#endif /* MLIBC_BUILDING_RTLD */

#if __MLIBC_POSIX_OPTION
SYSDEP_FUNC(Readv, int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read);
SYSDEP_FUNC(Writev, int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written);
SYSDEP_FUNC(Pwrite, int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_read);
SYSDEP_FUNC(Access, const char *path, int mode);
SYSDEP_FUNC(Faccessat, int dirfd, const char *pathname, int mode, int flags);
SYSDEP_FUNC(Dup, int fd, int flags, int *newfd);
SYSDEP_FUNC(Dup2, int fd, int flags, int newfd);
SYSDEP_FUNC(Statvfs, const char *path, struct statvfs *out);
SYSDEP_FUNC(Fstatvfs, int fd, struct statvfs *out);
SYSDEP_FUNC(Readlink, const char *path, void *buffer, size_t max_size, ssize_t *length);
SYSDEP_FUNC(Readlinkat, int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length);
SYSDEP_FUNC(Truncate, const char *path, off_t length);
SYSDEP_FUNC(Ftruncate, int fd, size_t size);
SYSDEP_FUNC(Fallocate, int fd, off_t offset, size_t size);
SYSDEP_FUNC(Openat, int dirfd, const char *path, int flags, mode_t mode, int *fd);
SYSDEP_FUNC(Socket, int family, int type, int protocol, int *fd);
SYSDEP_FUNC(MsgSend, int fd, const struct msghdr *hdr, int flags, ssize_t *length);
SYSDEP_FUNC(Sendto, int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length);
SYSDEP_FUNC(MsgRecv, int fd, struct msghdr *hdr, int flags, ssize_t *length);
SYSDEP_FUNC(Recvfrom, int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length);
SYSDEP_FUNC(Listen, int fd, int backlog);
SYSDEP_FUNC_RET(gid_t, GetGid);
SYSDEP_FUNC_RET(gid_t, GetEgid);
SYSDEP_FUNC_RET(uid_t, GetUid);
SYSDEP_FUNC_RET(uid_t, GetEuid);
SYSDEP_FUNC_RET(pid_t, GetTid);
SYSDEP_FUNC_RET(pid_t, GetPpid);
SYSDEP_FUNC(GetPgid, pid_t pid, pid_t *pgid);
SYSDEP_FUNC(GetSid, pid_t pid, pid_t *sid);
SYSDEP_FUNC(SetPgid, pid_t pid, pid_t pgid);
SYSDEP_FUNC(SetUid, uid_t uid);
SYSDEP_FUNC(SetEuid, uid_t euid);
SYSDEP_FUNC(SetGid, gid_t gid);
SYSDEP_FUNC(SetEgid, gid_t egid);
SYSDEP_FUNC(GetGroups, size_t size, gid_t *list, int *ret);
SYSDEP_FUNC(Fexecve, int fd, char *const argv[], char *const envp[]);
SYSDEP_FUNC(Pselect, int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events);
SYSDEP_FUNC(GetRusage, int scope, struct rusage *usage);
SYSDEP_FUNC(GetRlimit, int resource, struct rlimit *limit);
SYSDEP_FUNC(SetRlimit, int resource, const struct rlimit *limit);
SYSDEP_FUNC(GetPriority, int which, id_t who, int *value);
SYSDEP_FUNC(SetPriority, int which, id_t who, int prio);
SYSDEP_FUNC(GetSchedparam, void *tcb, int *policy, struct sched_param *param);
SYSDEP_FUNC(SetSchedparam, void *tcb, int policy, const struct sched_param *param);
SYSDEP_FUNC(GetScheduler, pid_t pid, int *policy);
SYSDEP_FUNC(SetScheduler, pid_t pid, int policy, const struct sched_param *param);
SYSDEP_FUNC(GetParam, pid_t pid, struct sched_param *param);
SYSDEP_FUNC(SetParam, pid_t pid, const struct sched_param *param);
SYSDEP_FUNC(GetMaxPriority, int policy, int *out);
SYSDEP_FUNC(GetMinPriority, int policy, int *out);
SYSDEP_FUNC(GetCwd, char *buffer, size_t size);
SYSDEP_FUNC(Chdir, const char *path);
SYSDEP_FUNC(Fchdir, int fd);
SYSDEP_FUNC(Chroot, const char *path);
SYSDEP_FUNC(Mkdir, const char *path, mode_t mode);
SYSDEP_FUNC(Mkdirat, int dirfd, const char *path, mode_t mode);
SYSDEP_FUNC(Link, const char *old_path, const char *new_path);
SYSDEP_FUNC(Linkat, int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags);
SYSDEP_FUNC(Symlink, const char *target_path, const char *link_path);
SYSDEP_FUNC(Symlinkat, const char *target_path, int dirfd, const char *link_path);
SYSDEP_FUNC(Renameat, int olddirfd, const char *old_path, int newdirfd, const char *new_path);
SYSDEP_FUNC(Fcntl, int fd, int request, va_list args, int *result);
SYSDEP_FUNC(Ttyname, int fd, char *buf, size_t size);
SYSDEP_FUNC(Fadvise, int fd, off_t offset, off_t length, int advice);
SYSDEP_FUNC_RET(void, Sync);
SYSDEP_FUNC(Fsync, int fd);
SYSDEP_FUNC(Fdatasync, int fd);
SYSDEP_FUNC(Chmod, const char *pathname, mode_t mode);
SYSDEP_FUNC(Fchmod, int fd, mode_t mode);
SYSDEP_FUNC(Fchmodat, int fd, const char *pathname, mode_t mode, int flags);
SYSDEP_FUNC(Utimensat, int dirfd, const char *pathname, const struct timespec times[2], int flags);
SYSDEP_FUNC(Mlock, const void *addr, size_t length);
SYSDEP_FUNC(Munlock, const void *addr, size_t length);
SYSDEP_FUNC(Mlockall, int flags);
SYSDEP_FUNC(Munlockall);
SYSDEP_FUNC(Mincore, void *addr, size_t length, unsigned char *vec);
SYSDEP_FUNC(VmRemap, void *pointer, size_t size, size_t new_size, void **window);
SYSDEP_FUNC(SetSid, pid_t *sid);
SYSDEP_FUNC(Tcgetattr, int fd, struct termios *attr);
SYSDEP_FUNC(Tcsetattr, int, int, const struct termios *attr);
SYSDEP_FUNC(Tcsendbreak, int fd, int dur);
SYSDEP_FUNC(Tcflow, int, int);
SYSDEP_FUNC(Tcflush, int fd, int queue);
SYSDEP_FUNC(Tcdrain, int);
SYSDEP_FUNC(Tcgetwinsize, int fd, struct winsize *winsz);
SYSDEP_FUNC(Tcsetwinsize, int fd, const struct winsize *winsz);
SYSDEP_FUNC(Pipe, int *fds, int flags);
SYSDEP_FUNC(Socketpair, int domain, int type_and_flags, int proto, int *fds);
SYSDEP_FUNC(Poll, struct pollfd *fds, nfds_t count, int timeout, int *num_events);
SYSDEP_FUNC(Ppoll, struct pollfd *fds, nfds_t count, const struct timespec *ts, const sigset_t *mask, int *num_events);
SYSDEP_FUNC(Ioctl, int fd, unsigned long request, void *arg, int *result);
SYSDEP_FUNC(PosixDevctl, int fd, int dcmd, void *__restrict dev_data_ptr, size_t nbyte, int *__restrict dev_info_ptr);
SYSDEP_FUNC(GetSockopt, int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size);
SYSDEP_FUNC(SetSockopt, int fd, int layer, int number, const void *buffer, socklen_t size);
SYSDEP_FUNC(Shutdown, int sockfd, int how);
SYSDEP_FUNC(Sockatmark, int sockfd, int *out);
SYSDEP_FUNC(ThreadSigmask, int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
SYSDEP_FUNC(Sigtimedwait, const sigset_t *__restrict set, siginfo_t *__restrict info, const struct timespec *__restrict timeout, int *out_signal);
SYSDEP_FUNC(Accept, int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags);
SYSDEP_FUNC(Bind, int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
SYSDEP_FUNC(Connect, int fd, const struct sockaddr *addr_ptr, socklen_t addr_length);
SYSDEP_FUNC(Sockname, int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
SYSDEP_FUNC(Peername, int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length);
SYSDEP_FUNC(GetHostname, char *buffer, size_t bufsize);
SYSDEP_FUNC(SetHostname, const char *buffer, size_t bufsize);
SYSDEP_FUNC(Mkfifoat, int dirfd, const char *path, mode_t mode);
SYSDEP_FUNC(GetEntropy, void *buffer, size_t length);
SYSDEP_FUNC(Mknodat, int dirfd, const char *path, int mode, int dev);
SYSDEP_FUNC(Umask, mode_t mode, mode_t *old);
SYSDEP_FUNC(BeforeCancellableSyscall, ucontext_t *uctx);
SYSDEP_FUNC(Tgkill, int tgid, int tid, int sig);
SYSDEP_FUNC(Fchownat, int dirfd, const char *pathname, uid_t owner, gid_t group, int flags);
SYSDEP_FUNC(Sigaltstack, const stack_t *ss, stack_t *oss);
SYSDEP_FUNC(Sigsuspend, const sigset_t *set);
SYSDEP_FUNC(Sigpending, sigset_t *set);
SYSDEP_FUNC(Sigqueue, pid_t pid, int sig, const union sigval val);
SYSDEP_FUNC(SetGroups, size_t size, const gid_t *list);
SYSDEP_FUNC(MemfdCreate, const char *name, int flags, int *fd);
SYSDEP_FUNC(Madvise, void *addr, size_t length, int advice);
SYSDEP_FUNC(PosixMadvise, void *addr, size_t length, int advice);
SYSDEP_FUNC(Msync, void *addr, size_t length, int flags);
SYSDEP_FUNC(GetItimer, int which, struct itimerval *curr_value);
SYSDEP_FUNC(SetItimer, int which, const struct itimerval *new_value, struct itimerval *old_value);
SYSDEP_FUNC(TimerCreate, clockid_t clk, struct sigevent *__restrict evp, timer_t *__restrict res);
SYSDEP_FUNC(TimerSettime, timer_t t, int flags, const struct itimerspec *__restrict val, struct itimerspec *__restrict old);
SYSDEP_FUNC(TimerGettime, timer_t t, struct itimerspec *val);
SYSDEP_FUNC(TimerDelete, timer_t t);
SYSDEP_FUNC(TimerGetoverrun, timer_t t, int *out);
SYSDEP_FUNC(Times, struct tms *tms, clock_t *out);
SYSDEP_FUNC(Uname, struct utsname *buf);
SYSDEP_FUNC(Pause);
SYSDEP_FUNC(SetResuid, uid_t ruid, uid_t euid, uid_t suid);
SYSDEP_FUNC(SetResgid, gid_t rgid, gid_t egid, gid_t sgid);
SYSDEP_FUNC(GetResuid, uid_t *ruid, uid_t *euid, uid_t *suid);
SYSDEP_FUNC(GetResgid, gid_t *rgid, gid_t *egid, gid_t *sgid);
SYSDEP_FUNC(SetReuid, uid_t ruid, uid_t euid);
SYSDEP_FUNC(SetRegid, gid_t rgid, gid_t egid);
SYSDEP_FUNC(GetLoginR, char *name, size_t name_len);
SYSDEP_FUNC(IfIndextoname, unsigned int index, char *name);
SYSDEP_FUNC(IfNametoindex, const char *name, unsigned int *ret);
SYSDEP_FUNC(Ptsname, int fd, char *buffer, size_t length);
SYSDEP_FUNC(Unlockpt, int fd);
SYSDEP_FUNC(ThreadSetname, void *tcb, const char *name);
SYSDEP_FUNC(ThreadGetname, void *tcb, char *name, size_t size);
SYSDEP_FUNC(Sysconf, int num, long *ret);
SYSDEP_FUNC(Semget, key_t key, int n, int fl, int *id);
SYSDEP_FUNC(Semctl, int semid, int semnum, int cmd, void *semun, int *ret);
SYSDEP_FUNC(GetAffinity, pid_t pid, size_t cpusetsize, cpu_set_t *mask);
SYSDEP_FUNC(GetThreadaffinity, pid_t tid, size_t cpusetsize, cpu_set_t *mask);
SYSDEP_FUNC(SetAffinity, pid_t pid, size_t cpusetsize, const cpu_set_t *mask);
SYSDEP_FUNC(SetThreadaffinity, pid_t tid, size_t cpusetsize, const cpu_set_t *mask);
SYSDEP_FUNC(GetCurrentStackInfo, void **stack_base, size_t *stack_size);
SYSDEP_FUNC(Waitid, idtype_t idtype, id_t id, siginfo_t *info, int options);
SYSDEP_FUNC(NameToHandleAt, int dirfd, const char *pathname, struct file_handle *handle, int *mount_id, int flags);
SYSDEP_FUNC(Splice, int in_fd, off_t *in_off, int out_fd, off_t *out_off, size_t size, unsigned int flags, ssize_t *out);
SYSDEP_FUNC(Shmat, void **seg_start, int shmid, const void *shmaddr, int shmflg);
SYSDEP_FUNC(Shmctl, int *idx, int shmid, int cmd, struct shmid_ds *buf);
SYSDEP_FUNC(Shmdt, const void *shmaddr);
SYSDEP_FUNC(Shmget, int *shm_id, key_t key, size_t size, int shmflg);
SYSDEP_FUNC(InetConfigured, bool *ipv4, bool *ipv6);
SYSDEP_FUNC(Nice, int nice, int *new_nice);
SYSDEP_FUNC(Openpt, int oflags, int *fd);
SYSDEP_FUNC(Msgctl, int q, int cmd, struct msqid_ds *buf);
SYSDEP_FUNC(Msgget, key_t k, int flag, int *out);
SYSDEP_FUNC(Msgrcv, int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out);
SYSDEP_FUNC(Msgsnd, int msqid, const void *msgp, size_t msgsz, int msgflg);
SYSDEP_FUNC(MqOpen, const char *name, int oflag, mode_t mode, mq_attr *attr, mqd_t *out);
SYSDEP_FUNC(MqUnlink, const char *name);
SYSDEP_FUNC(MqReceive, mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);
SYSDEP_FUNC(MqGetAttr, mqd_t mqdes, mq_attr *mqstat);
SYSDEP_FUNC(MqSetAttr, mqd_t mqdes, const mq_attr *mqstat, mq_attr *omqstat);
#endif // __MLIBC_POSIX_OPTION

#if __MLIBC_LINUX_OPTION
SYSDEP_FUNC(InotifyCreate, int flags, int *fd);
SYSDEP_FUNC(InotifyAddWatch, int ifd, const char *path, uint32_t mask, int *wd);
SYSDEP_FUNC(InotifyRmWatch, int ifd, int wd);
SYSDEP_FUNC(Mount, const char *source, const char *target, const char *fstype, unsigned long flags, const void *data);
SYSDEP_FUNC(Umount2, const char *target, int flags);
SYSDEP_FUNC(Ptrace, long req, pid_t pid, void *addr, void *data, long *out);
SYSDEP_FUNC(Capget, cap_user_header_t hdrp, cap_user_data_t datap);
SYSDEP_FUNC(Capset, cap_user_header_t hdrp, const cap_user_data_t datap);
SYSDEP_FUNC(Prctl, int option, va_list va, int *out);
SYSDEP_FUNC(InitModule, void *module, unsigned long length, const char *args);
SYSDEP_FUNC(DeleteModule, const char *name, unsigned flags);
SYSDEP_FUNC(Klogctl, int type, char *bufp, int len, int *out);
SYSDEP_FUNC(Getcpu, int *cpu);
SYSDEP_FUNC(Sysinfo, struct sysinfo *info);
SYSDEP_FUNC(Swapon, const char *path, int flags);
SYSDEP_FUNC(Swapoff, const char *path);
SYSDEP_FUNC(Setxattr, const char *path, const char *name, const void *val, size_t size, int flags);
SYSDEP_FUNC(Lsetxattr, const char *path, const char *name, const void *val, size_t size, int flags);
SYSDEP_FUNC(Fsetxattr, int fd, const char *name, const void *val, size_t size, int flags);
SYSDEP_FUNC(Getxattr, const char *path, const char *name, void *val, size_t size, ssize_t *nread);
SYSDEP_FUNC(Lgetxattr, const char *path, const char *name, void *val, size_t size, ssize_t *nread);
SYSDEP_FUNC(Fgetxattr, int fd, const char *name, void *val, size_t size, ssize_t *nread);
SYSDEP_FUNC(Listxattr, const char *path, char *list, size_t size, ssize_t *nread);
SYSDEP_FUNC(Llistxattr, const char *path, char *list, size_t size, ssize_t *nread);
SYSDEP_FUNC(Flistxattr, int fd, char *list, size_t size, ssize_t *nread);
SYSDEP_FUNC(Removexattr, const char *path, const char *name);
SYSDEP_FUNC(Lremovexattr, const char *path, const char *name);
SYSDEP_FUNC(Fremovexattr, int fd, const char *name);
SYSDEP_FUNC(Statfs, const char *path, struct statfs *buf);
SYSDEP_FUNC(Fstatfs, int fd, struct statfs *buf);
SYSDEP_FUNC(Statx, int dirfd, const char *pathname, int flags, unsigned int mask, struct statx *statxbuf);
SYSDEP_FUNC(Getifaddrs, struct ifaddrs **);
SYSDEP_FUNC(Sendfile, int outfd, int infd, off_t *offset, size_t count, ssize_t *out);
SYSDEP_FUNC(Syncfs, int fd);
SYSDEP_FUNC(Unshare, int flags);
SYSDEP_FUNC(SetNs, int fd, int nstype);
SYSDEP_FUNC(PidfdOpen, pid_t pid, unsigned int flags, int *outfd);
SYSDEP_FUNC(PidfdGetpid, int fd, pid_t *outpid);
SYSDEP_FUNC(PidfdSendSignal, int pidfd, int sig, siginfo_t *info, unsigned int flags);
SYSDEP_FUNC(ProcessVmReadv, pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
SYSDEP_FUNC(ProcessVmWritev, pid_t pid, const struct iovec *local_iov, unsigned long liovcnt, const struct iovec *remote_iov, unsigned long riovcnt, unsigned long flags, ssize_t *out);
SYSDEP_FUNC(Fsopen, const char *fsname, unsigned int flags, int *outfd);
SYSDEP_FUNC(Fsmount, int fsfd, unsigned int flags, unsigned int mountflags, int *outfd);
SYSDEP_FUNC(Fsconfig, int fd, unsigned int cmd, const char *key, const void *val, int aux);
SYSDEP_FUNC(MoveMount, int from_dirfd, const char *from_path, int to_dirfd, const char *to_path, unsigned int flags);
SYSDEP_FUNC(OpenTree, int dirfd, const char *path, unsigned int flags, int *outfd);
SYSDEP_FUNC(CopyFileRange, int fd_in, off_t *off_in, int fd_out, off_t *off_out, size_t count, unsigned int flags, ssize_t *bytes_copied);
#endif // __MLIBC_LINUX_OPTION

#if __MLIBC_LINUX_EPOLL_OPTION
SYSDEP_FUNC(EpollCreate, int flags, int *fd);
SYSDEP_FUNC(EpollCtl, int epfd, int mode, int fd, struct epoll_event *ev);
SYSDEP_FUNC(EpollPwait, int epfd, struct epoll_event *ev, int n, int timeout, const sigset_t *sigmask, int *raised);
#endif // __MLIBC_LINUX_EPOLL_OPTION

#if __MLIBC_LINUX_TIMERFD_OPTION
SYSDEP_FUNC(TimerfdCreate, int clockid, int flags, int *fd);
SYSDEP_FUNC(TimerfdSettime, int fd, int flags, const struct itimerspec *value, struct itimerspec *oldvalue);
SYSDEP_FUNC(TimerfdGettime, int fd, struct itimerspec *its);
#endif // __MLIBC_LINUX_TIMERFD_OPTION

#if __MLIBC_LINUX_SIGNALFD_OPTION
SYSDEP_FUNC(SignalfdCreate, const sigset_t *, int flags, int *fd);
#endif // __MLIBC_LINUX_SIGNALFD_OPTION

#if __MLIBC_LINUX_EVENTFD_OPTION
SYSDEP_FUNC(EventfdCreate, unsigned int initval, int flags, int *fd);
#endif // __MLIBC_LINUX_EVENTFD_OPTION

#if __MLIBC_LINUX_REBOOT_OPTION
SYSDEP_FUNC(Reboot, int cmd);
#endif // __MLIBC_LINUX_REBOOT_OPTION

#if __MLIBC_LINUX_WRAPPERS_OPTION
SYSDEP_FUNC(RtSigqueueinfo, pid_t tgid, int sig, siginfo_t *uinfo);
SYSDEP_FUNC(RtTgSigqueueinfo, pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo);
#endif // __MLIBC_LINUX_WRAPPERS_OPTION

#if __MLIBC_BSD_OPTION
SYSDEP_FUNC(Brk, void **out);
SYSDEP_FUNC(GetLoadavg, double *samples);
SYSDEP_FUNC(Openpty, int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win);
#endif // __MLIBC_BSD_OPTION

#if __MLIBC_GLIBC_OPTION
SYSDEP_FUNC(Personality, unsigned long persona, int *out);
SYSDEP_FUNC(Ioperm, unsigned long int from, unsigned long int num, int turn_on);
SYSDEP_FUNC(Iopl, int level);
#ifdef __riscv
SYSDEP_FUNC(RiscvFlushIcache, void *start, void *end, unsigned long flags);
#endif // __riscv
#endif // __MLIBC_GLIBC_OPTION

#undef SYSDEP_FUNC
#undef SYSDEP_FUNC_RET
#undef SYSDEP_FUNC_NORETURN

} // namespace mlibc

#endif /* MLIBC_SYSDEP_SIGNATURES */
