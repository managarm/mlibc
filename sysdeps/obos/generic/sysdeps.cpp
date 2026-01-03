#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/stat.h>

#include <frg/logging.hpp>
#include <mlibc/debug.hpp>

#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <obos/error.h>
#include <obos/syscall.h>
#include <obos/vma.h>

#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/utsname.h>
#include <abi-bits/vm-flags.h>
#include <unistd.h>

#include <bits/ensure.h>

extern "C" int obos_fork(uint32_t* child);

namespace {
	int parse_file_status(obos_status status);
	int interpret_signal_status(obos_status status);
	int parse_pgid_status(obos_status status);
} /* namespace */

namespace mlibc {

#define define_stub(signature, ret)                                                                \
	signature {                                                                                    \
		mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!\n" << frg::endlog;            \
		return (ret);                                                                              \
	}

int sys_sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	if (how > 3)
		return EINVAL;
	// 'how' in oboskrnl has the same values as in Linux (the abi we "borrow" from).
	return interpret_signal_status((obos_status)syscall3(Sys_SigProcMask, how, set, retrieve));
}
int sys_sigpending(sigset_t *set) {
	return interpret_signal_status((obos_status)syscall1(Sys_SigPending, set));
}

int sys_uname(struct utsname *out) {
	memcpy(out->sysname, "OBOS", 4);
	sys_gethostname(out->nodename, 65);
	memcpy(out->release, "v0.0.0", 7);
	memcpy(out->version, "UNKNOWN", 8);
#if defined(__x86_64__)
	memcpy(out->machine, "x86_64", 7);
#elif defined(__m68k__)
	memcpy(out->machine, "m68k", 5);
#else
	return ENOSYS;
#endif
	return 0;
}

#ifndef MLIBC_BUILDING_RTLD

extern "C" void __mlibc_restorer();

typedef struct user_sigaction {
	union {
		void (*handler)(int signum);
		void (*fn_sigaction)(int signum, siginfo_t *info, void *unknown);
	} un;
	// NOTE(oberrow): Set to __mlibc_restorer in the mlibc sysdeps.
	uintptr_t trampoline_base; // required
	uint32_t flags;
} user_sigaction;

int sys_sigaction(
    int sigval,
    const struct sigaction *__restrict newact_mlibc,
    struct sigaction *__restrict oldact_mlibc
) {
	user_sigaction newact = {}, oldact = {};
	if (newact_mlibc) {
		newact.un.handler = newact_mlibc->sa_handler;
		newact.flags = newact_mlibc->sa_flags;
		newact.trampoline_base = (uintptr_t)__mlibc_restorer;
	}
	int err = interpret_signal_status((obos_status)syscall3(
	    Sys_SigAction, sigval, newact_mlibc ? &newact : nullptr, oldact_mlibc ? &oldact : nullptr
	));
	if (oldact_mlibc && !err) {
		oldact_mlibc->sa_handler = oldact.un.handler;
		oldact_mlibc->sa_flags = oldact.flags;
		oldact_mlibc->sa_restorer = (void (*)())oldact.trampoline_base;
	}
	return err;
}

#endif

int sys_dup(int fd, int flags, int *newfd) {
	(void)flags;
	*newfd = HANDLE_ANY;
	return parse_file_status((obos_status)syscall2(Sys_HandleClone, fd, newfd));
}
int sys_dup2(int fd, int flags, int newfd) {
	(void)flags;
	return parse_file_status((obos_status)syscall2(Sys_HandleClone, fd, &newfd));
}

int sys_mkfifoat(int dirfd, const char *path, mode_t mode) {
	return parse_file_status((obos_status)syscall4(Sys_CreateNamedPipe, dirfd, path, mode, 0));
}

int sys_fchownat(int dirfd, const char *pathname, uid_t owner, gid_t group, int flags) {
	return parse_file_status(
	    (obos_status)syscall5(Sys_FChownAt, dirfd, pathname, owner, group, flags)
	);
}

// idk why the mlibc maintainers decided they wanted [f]chown[at] to only use one sysdep, while
// [f]chmod[at] has 3 separate sysdeps

int sys_chmod(const char *pathname, mode_t mode) {
	return sys_fchmodat(AT_FDCWD, pathname, mode, 0);
}
int sys_fchmod(int fd, mode_t mode) { return sys_fchmodat(fd, "", mode, AT_EMPTY_PATH); }
int sys_fchmodat(int fd, const char *pathname, mode_t mode, int flags) {
	return parse_file_status((obos_status)syscall4(Sys_FChmodAt, fd, pathname, mode, flags));
}

int sys_sigaltstack(const stack_t *ss, stack_t *oss) {
	// stack_t is the same on obos and on Linux (the abi we "borrow" from)
	return interpret_signal_status((obos_status)syscall2(Sys_SigAltStack, ss, oss));
}

int sys_kill(pid_t pid, int sigval) {
	if (pid == -1)
		return ENOSYS;
	if (pid == 0) {
		pid_t pgid = 0;
		sys_getpgid(0, &pgid);
		return interpret_signal_status((obos_status)syscall2(Sys_KillProcessGroup, pgid, sigval));
	}
	if (pid < -1)
		return interpret_signal_status((obos_status)syscall2(Sys_KillProcessGroup, -pid, sigval));
	handle hnd = (handle)syscall1(Sys_ProcessOpen, pid);
	if (hnd == HANDLE_INVALID)
		return ESRCH;
	int err = interpret_signal_status((obos_status)syscall2(Sys_KillProcess, hnd, sigval));
	syscall1(Sys_HandleClose, hnd);
	return err;
}

int sys_tgkill(int, int tid, int sigval) {
	// TODO: Thread groups?
	handle thread = syscall2(Sys_ThreadOpen, HANDLE_CURRENT, tid);
	if (thread == HANDLE_INVALID)
		return ESRCH;
	int err = interpret_signal_status((obos_status)syscall2(Sys_Kill, thread, sigval));
	syscall1(Sys_HandleClose, thread);
	return err;
}

int sys_utimensat(int dirfd, const char *pathname, const struct timespec times[2], int flags) {
	return parse_file_status((obos_status)syscall4(Sys_UTimeNSAt, dirfd, pathname, times, flags));
}

int sys_waitpid(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	// TODO(oberrow): struct rusage and pid values that are < -1 or zero
	if (ru) {
		mlibc::infoLogger() << "mlibc: " << __func__
		                    << "struct rusage in sys_waitpid is unsupported" << frg::endlog;
		return ENOSYS;
	}
	if (pid < -1 || pid == 0) {
		mlibc::infoLogger() << "mlibc:" << __func__ << " pid value " << pid << " is unsupported"
		                    << frg::endlog;
		return ENOSYS;
	}
	int ec = 0;
	handle hnd = pid == -1 ? HANDLE_ANY : (handle)syscall1(Sys_ProcessOpen, pid);
	if (hnd == HANDLE_INVALID)
		return ESRCH;
	obos_status st = (obos_status)syscall4(Sys_WaitProcess, hnd, status, flags, ret_pid);
	switch (st) {
		case OBOS_STATUS_INVALID_ARGUMENT:
			ec = EINVAL;
			goto exit;
		case OBOS_STATUS_ABORTED:
			ec = EINTR;
			goto exit;
		case OBOS_STATUS_NOT_FOUND:
			ec = ECHILD;
			goto exit;
		default:
			break;
	}
exit:
	if (hnd != HANDLE_ANY)
		syscall1(Sys_HandleClose, hnd);
	return ec;
}

// define_stub(int sys_isatty(int fd), 1)

// Architecture-specific.
#ifndef MLIBC_BUILDING_RTLD
int sys_fork(pid_t* out_child)
{
	// Do this in case pid_t changes
	// to a different-sized integer in
	// the ABI.
	uint32_t child = 0;
	int ret = obos_fork(&child);
	*out_child = child;
	return ret;
}
#else
int sys_fork(pid_t*)
{
	return ENOSYS;
}
#endif

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
	obos_status st = (obos_status)syscall3(Sys_ExecVE, path, argv, envp);
	switch (st) {
		case OBOS_STATUS_UNIMPLEMENTED:
			return ENOSYS;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_PAGE_FAULT:
			return EFAULT;
		case OBOS_STATUS_INVALID_FILE:
			return ENOEXEC;
		case OBOS_STATUS_NOT_FOUND:
			return ENOENT;
		default:
			return parse_file_status(st);
	}
}

int sys_chdir(const char *path) {
	return parse_file_status((obos_status)syscall1(Sys_Chdir, path));
}
int sys_fchdir(int fd) { return parse_file_status((obos_status)syscall1(Sys_ChdirEnt, fd)); }
int sys_getcwd(char *buffer, size_t size) {
	return parse_file_status((obos_status)syscall2(Sys_GetCWD, buffer, size));
}

void sys_libc_log(char const *str) { syscall1(Sys_LibCLog, str); }

int sys_get_current_stack_info(void **stack_base, size_t *stack_size)
{
	obos_status status = (obos_status)syscall3(Sys_ThreadGetStack, HANDLE_CURRENT, stack_base, stack_size);
	if (obos_is_success(status))
		return 0;
	switch (status)
	{
		case OBOS_STATUS_INVALID_ARGUMENT: return EINVAL;
		case OBOS_STATUS_UNIMPLEMENTED: return ENOSYS;
		default: return EIO;
	}
}

uid_t sys_getuid() { return (uid_t)syscall0(Sys_GetUid); }

uid_t sys_geteuid() {
	uid_t res = 0;
	sys_getresuid(nullptr, &res, nullptr);
	return res;
}

int sys_getresuid(uid_t *ruid, uid_t *euid, uid_t *suid) {
	return parse_file_status((obos_status)syscall3(Sys_GetRESUid, ruid, euid, suid));
}
int sys_setresuid(uid_t ruid, uid_t euid, uid_t suid) {
	return parse_file_status((obos_status)syscall3(Sys_SetRESUid, ruid, euid, suid));
}

gid_t sys_getgid() { return (gid_t)syscall0(Sys_GetGid); }

gid_t sys_getegid() {
	gid_t res = 0;
	sys_getresgid(nullptr, &res, nullptr);
	return res;
}

int sys_getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	return parse_file_status((obos_status)syscall3(Sys_GetRESGid, rgid, egid, sgid));
}
int sys_setresgid(gid_t rgid, gid_t egid, gid_t sgid) {
	return parse_file_status((obos_status)syscall3(Sys_SetRESGid, rgid, egid, sgid));
}

int sys_setreuid(uid_t ruid, uid_t euid) { return sys_setresuid(ruid, euid, -1); }
int sys_setregid(gid_t rgid, gid_t egid) { return sys_setresgid(rgid, egid, -1); }

int sys_setuid(uid_t uid) { return parse_file_status((obos_status)syscall1(Sys_SetUid, uid)); }
int sys_seteuid(uid_t euid) { return sys_setresuid(-1, euid, -1); }
int sys_setgid(gid_t gid) { return parse_file_status((obos_status)syscall1(Sys_SetGid, gid)); }
int sys_setegid(gid_t egid) { return sys_setresgid(-1, egid, -1); }

int sys_setgroups(size_t size, const gid_t *list) {
	return parse_file_status((obos_status)syscall3(Sys_SetGroups, HANDLE_CURRENT, size, list));
}
int sys_getgroups(size_t size, gid_t *list, int *ret) {
	if (!ret)
		return EINVAL;
	size_t sz_ret = size;
	int ec = parse_file_status((obos_status)syscall3(Sys_GetGroups, HANDLE_CURRENT, &sz_ret, list));
	*ret = (int)sz_ret;
	return ec;
}

pid_t sys_gettid() { return (pid_t)syscall1(Sys_ThreadGetTid, HANDLE_CURRENT); }
pid_t sys_getpid() { return (pid_t)syscall1(Sys_ProcessGetPID, HANDLE_CURRENT); }
pid_t sys_getppid() { return (pid_t)syscall1(Sys_ProcessGetPPID, HANDLE_CURRENT); }

#define HANDLE_TYPE_INVALID 0xff

int sys_getpgid(pid_t pid, pid_t *pgid) {
	handle hnd = pid != 0 ? (handle)syscall1(Sys_ProcessOpen, pid) : HANDLE_CURRENT;
	if (HANDLE_TYPE(hnd) == HANDLE_TYPE_INVALID)
		return ESRCH;
	int ec = parse_pgid_status((obos_status)syscall2(Sys_GetProcessGroup, hnd, pgid));
	if (hnd != HANDLE_CURRENT)
		syscall1(Sys_HandleClose, hnd);
	return ec;
}

int sys_setpgid(pid_t pid, pid_t pgid) {
	handle hnd = pid != 0 ? (handle)syscall1(Sys_ProcessOpen, pid) : HANDLE_CURRENT;
	if (HANDLE_TYPE(hnd) == HANDLE_TYPE_INVALID)
		return ESRCH;
	int ec = parse_pgid_status((obos_status)syscall2(Sys_SetProcessGroup, hnd, pgid));
	if (hnd != HANDLE_CURRENT)
		syscall1(Sys_HandleClose, hnd);
	return ec;
}

// Sanity check
static_assert(sizeof(pid_t) == sizeof(uint32_t), "unsupported pid_t size");

int sys_setsid(pid_t *sid) {
	return parse_pgid_status((obos_status)syscall2(Sys_SetSid, HANDLE_CURRENT, sid));
}

int sys_getsid(pid_t pid, pid_t *sid) {
	handle hnd = pid != 0 ? (handle)syscall1(Sys_ProcessOpen, pid) : HANDLE_CURRENT;
	if (HANDLE_TYPE(hnd) == HANDLE_TYPE_INVALID)
		return ESRCH;
	int ec = parse_pgid_status((obos_status)syscall2(Sys_GetSid, hnd, sid));
	if (hnd != HANDLE_CURRENT)
		syscall1(Sys_HandleClose, hnd);
	return ec;
}

[[noreturn]] void sys_libc_panic() {
	sys_libc_log("mlibc panicked! exiting program...\n");
	sys_exit(-1);
	__builtin_unreachable();
}

[[noreturn]] void sys_exit(int status) {
	syscall1(Sys_ExitCurrentProcess, status);
	__builtin_unreachable();
}

[[noreturn]] void sys_thread_exit() { syscall0(Sys_ExitCurrentThread); }

int sys_futex_wait(int *pointer, int expected, const struct timespec *time) {
	obos_status status = (obos_status)syscall3(
	    Sys_FutexWait, pointer, expected, time ? time->tv_sec * 1000 : UINT64_MAX
	);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_RETRY:
		case OBOS_STATUS_TIMED_OUT:
			return EAGAIN;
		case OBOS_STATUS_ABORTED:
			return EINTR;
		default:
			sys_libc_panic(); // TODO: Generic errno value?
	}
}

int sys_futex_wake(int *pointer, bool all) {
	obos_status status = (obos_status)syscall2(Sys_FutexWake, pointer, all ? UINT32_MAX : 1);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		default:
			return ENOSYS;
	}
}

int sys_clock_get(int clock, time_t *secs, long *nanos) {
#if defined(__x86_64__)
	static struct {
		long rtc_time_offset;
		uint64_t rtc_time_offset_rel; // the time rtc_time_offset was recorded at.
		uint64_t tsc_frequency;
		double factor;
		bool tsc_exists;
		bool tsc_probed;
	} timing_info;

	if (__builtin_expect(!timing_info.tsc_probed, false)) {
		timing_info.tsc_probed = true;
		timing_info.tsc_frequency = syscall0(SysS_QueryTSCFrequency);
		if (timing_info.tsc_frequency != UINT64_MAX && timing_info.tsc_frequency != OBOS_STATUS_UNIMPLEMENTED) {
			syscall3(SysS_ClockGet, 0, nullptr, &timing_info.rtc_time_offset);
			timing_info.rtc_time_offset_rel = __builtin_ia32_rdtsc();
			timing_info.tsc_exists = true;
			timing_info.factor = 1000000000 / timing_info.tsc_frequency;
		}
		else {
			timing_info.tsc_exists = false;
		}
	}

	if (__builtin_expect(timing_info.tsc_exists, true)) {
		switch (clock) {
			case 0:
			{
				uint64_t tstamp = __builtin_ia32_rdtsc() - timing_info.rtc_time_offset_rel;
				tstamp *= timing_info.factor;

				*nanos = timing_info.rtc_time_offset + tstamp;
				*secs = (timing_info.rtc_time_offset + tstamp) / 1000000000;
				break;
			}
			case 1:
			{
				*nanos = __builtin_ia32_rdtsc();
				*secs = *nanos / 1000000000;
				break;
			}
			default:
			    return ENOSYS;
		}

		return 0;
	}

	// NOTE(oberrow): This clock is not precise.
	obos_status st = (obos_status)syscall3(SysS_ClockGet, clock, secs, nanos);
	switch (st) {
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_PAGE_FAULT:
			return EFAULT;
		case OBOS_STATUS_SUCCESS:
			return 0;
		default:
			return ENOSYS;
	}
#else
	return ENOSYS;
#endif
}

int sys_link(const char *old_path, const char *new_path) {
	return sys_linkat(AT_FDCWD, old_path, AT_FDCWD, new_path, 0);
}

int sys_linkat(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	return parse_file_status(
	    (obos_status)syscall5(Sys_LinkAt, olddirfd, old_path, newdirfd, new_path, flags)
	);
}

// Copied from linux sysdep.

#define TIOCGPTN 0x80045430U
#define TIOCSPTLCK 0x40045431U

#ifndef MLIBC_BUILDING_RTLD
int sys_ptsname(int fd, char *buffer, size_t length) {
	int index;
	if(int e = sys_ioctl(fd, TIOCGPTN, &index, nullptr); e)
		return e;
	if((size_t)snprintf(buffer, length, "/dev/pts/%d", index) >= length) {
		return ERANGE;
	}
	return 0;
}
#endif

int sys_unlockpt(int fd) {
	int unlock = 0;

	if(int e = sys_ioctl(fd, TIOCSPTLCK, &unlock, nullptr); e)
		return e;

	return 0;
}

int sys_symlink(const char *target_path, const char *link_path) {
	return parse_file_status((obos_status)syscall2(Sys_SymLink, target_path, link_path));
}
int sys_symlinkat(const char *target_path, int dirfd, const char *link_path) {
	return parse_file_status((obos_status)syscall3(Sys_SymLinkAt, target_path, dirfd, link_path));
}

int sys_rename(const char *path, const char *new_path) {
	return sys_renameat(AT_FDCWD, path, AT_FDCWD, new_path);
}
int sys_renameat(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	return parse_file_status(
	    (obos_status)syscall4(Sys_RenameAt, olddirfd, old_path, newdirfd, new_path)
	);
}

int sys_open_dir(const char *path, int *hnd) {
	obos_status st = OBOS_STATUS_SUCCESS;
	handle dir = (handle)syscall2(Sys_OpenDir, path, &st);
	if (obos_is_error(st))
		return parse_file_status(st);
	*hnd = (int)dir;
	return 0;
}

int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	return parse_file_status(
	    (obos_status)syscall4(Sys_ReadEntries, handle, buffer, max_size, bytes_read)
	);
}

/*
int sys_pselect(int num_fds, fd_set *read_set, fd_set *write_set, fd_set *except_set, const struct
timespec *timeout, const sigset_t *sigmask, int *num_events)
{
    (void)(num_fds && read_set && write_set && except_set && timeout && sigmask);
    *num_events = 1;
    return 0;
}*/

int sys_pselect(
    int num_fds,
    fd_set *read_set,
    fd_set *write_set,
    fd_set *except_set,
    const struct timespec *timeout,
    const sigset_t *sigmask,
    int *num_events
) {
	uintptr_t tm = timeout ? timeout->tv_nsec / 1000 : UINTPTR_MAX;
	struct pselect_extra_args {
		const uintptr_t *timeout;
		const sigset_t *sigmask;
		int *num_events;
	} extra = {
	    .timeout = (tm != UINTPTR_MAX ? &tm : nullptr), .sigmask = sigmask, .num_events = num_events
	};
	return parse_file_status(
	    (obos_status)syscall5(Sys_PSelect, num_fds, read_set, write_set, except_set, &extra)
	);
}

int sys_poll(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	uintptr_t obos_timeout = 0;
	if (timeout < 0)
		obos_timeout = UINTPTR_MAX;
	else
		obos_timeout = timeout;
	return parse_file_status(
	    (obos_status)syscall5(Sys_PPoll, fds, count, &obos_timeout, nullptr, num_events)
	);
}

int sys_fcntl(int fd, int request, va_list vargs, int *result) {
	uintptr_t arg = va_arg(vargs, uintptr_t);
	return parse_file_status((obos_status)syscall5(Sys_Fcntl, fd, request, &arg, 1, result));
}

int sys_isatty(int fd) { return parse_file_status((obos_status)syscall1(Sys_IsATTY, fd)); }

int sys_ttyname(int fd, char *buf, size_t size) {
	return parse_file_status((obos_status)syscall3(Sys_TTYName, fd, buf, size));
}

#define TTY_IOCTL_SETATTR 0x01
#define TTY_IOCTL_GETATTR 0x02
#define TTY_IOCTL_FLOW 0x03
#define TTY_IOCTL_FLUSH 0x04
#define TTY_IOCTL_DRAIN 0x05

int sys_tcgetattr(int fd, struct termios *attr) {
	return parse_file_status(
	    (obos_status)syscall4(Sys_FdIoctl, fd, TTY_IOCTL_GETATTR, attr, sizeof(*attr))
	);
}
int sys_tcsetattr(int fd, int ign, const struct termios *attr) {
	(void)ign;
	return parse_file_status(
	    (obos_status)syscall4(Sys_FdIoctl, fd, TTY_IOCTL_SETATTR, attr, sizeof(*attr))
	);
}
int sys_tcflow(int fd, int ehow) {
	uint32_t how = ehow;
	return parse_file_status(
	    (obos_status)syscall4(Sys_FdIoctl, fd, TTY_IOCTL_FLOW, &how, sizeof(how))
	);
}
int sys_tcflush(int fd, int queue) {
	(void)(fd && queue);
	return ENOSYS;
}
int sys_tcdrain(int fd) {
	return parse_file_status((obos_status)syscall4(Sys_FdIoctl, fd, TTY_IOCTL_DRAIN, nullptr, 0));
}

int sys_ioctl(int fd, unsigned long request, void *arg, int *result) {
	int res = parse_file_status((obos_status)syscall4(Sys_FdIoctl, fd, request, arg, SIZE_MAX));
	if (result)
		*result = 0;
	return res;
}

int sys_pipe(int *fds, [[maybe_unused]] int flags) {
	int ec = parse_file_status((obos_status)syscall2(Sys_CreatePipe, fds, 0));
#ifndef MLIBC_BUILDING_RTLD
	if (ec == 0) {
		if (flags & O_NONBLOCK) {
			int fl = fcntl(fds[0], F_GETFL) | O_NONBLOCK;
			fcntl(fds[0], F_SETFL, fl);
			fl = fcntl(fds[1], F_GETFL) | O_NONBLOCK;
			fcntl(fds[1], F_SETFL, fl);
		}
	}
#endif
	return ec;
}

int sys_openat(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	(void)mode;
	int real_flags = 0;

	if ((flags & 3) == O_RDONLY)
		real_flags |= 1 /*FD_OFLAGS_READ*/;
	if ((flags & 3) == O_WRONLY)
		real_flags |= 2 /*FD_OFLAGS_WRITE*/;
	if ((flags & 3) == O_RDWR)
		real_flags |= 1 | 2 /*FD_OFLAGS_READ|FD_OFLAGS_WRITE*/;
	if (flags & O_CLOEXEC)
		real_flags |= 8 /* FD_OFLAGS_NOEXEC */;
	if (flags & O_DIRECT)
		real_flags |= 4 /* FD_OFLAGS_UNCACHED */;
	if (flags & O_CREAT)
		real_flags |= 16 /* FD_OFLAGS_CREATE */;
	if (flags & O_NOCTTY)
		real_flags |= 64 /* FD_OFLAGS_NOCTTY */;

	handle hnd = syscall0(Sys_FdAlloc);
	obos_status st = (obos_status)syscall5(Sys_FdOpenAtEx, hnd, dirfd, path, real_flags, mode);
	if (st == OBOS_STATUS_NOT_A_FILE && dirfd == AT_FDCWD)
		return sys_open_dir(path, fd);
	if (int ec = parse_file_status(st); ec != 0)
		return ec;
	*fd = hnd;

	if (flags & O_NONBLOCK)
		syscall5(Sys_Fcntl, hnd, F_SETFL, &flags, 1, nullptr);

	return 0;
}

int sys_umask(mode_t mode, mode_t *old) {
	syscall2(Sys_UMask, mode, old);
	return 0;
}

int sys_open(const char *pathname, int flags, mode_t mode, int *fd) {
	if (flags & O_DIRECTORY)
		return sys_open_dir(pathname, fd);

	handle hnd = (handle)syscall0(Sys_FdAlloc);
	uint32_t real_flags = 0;

	if ((flags & 3) == O_RDONLY)
		real_flags |= 1 /*FD_OFLAGS_READ*/;
	if ((flags & 3) == O_WRONLY)
		real_flags |= 2 /*FD_OFLAGS_WRITE*/;
	if ((flags & 3) == O_RDWR)
		real_flags |= 1 | 2 /*FD_OFLAGS_READ|FD_OFLAGS_WRITE*/;
	if (flags & O_CLOEXEC)
		real_flags |= 8 /* FD_OFLAGS_NOEXEC */;
	if (flags & O_DIRECT)
		real_flags |= 4 /* FD_OFLAGS_UNCACHED */;
	if (flags & O_CREAT)
		real_flags |= 16 /* FD_OFLAGS_CREATE */;
	if (flags & O_NOCTTY)
		real_flags |= 64 /* FD_OFLAGS_NOCTTY */;

	obos_status st = OBOS_STATUS_SUCCESS;

	st = (obos_status)syscall4(Sys_FdOpenEx, hnd, pathname, real_flags, mode);
	if (st == OBOS_STATUS_NOT_A_FILE) {
		syscall1(Sys_HandleClose, hnd);
		return sys_open_dir(pathname, fd);
	}

	if (int ec = parse_file_status(st); ec != 0)
		return ec;
	*fd = hnd;

	if (flags & O_NONBLOCK)
		syscall5(Sys_Fcntl, hnd, F_SETFL, &flags, 1, nullptr);

	return 0;
}

int sys_stat(fsfd_target fsfdt, int fd, const char *path, int flags, struct stat *statbuf) {
	return parse_file_status((obos_status)syscall5(Sys_Stat, fsfdt, fd, path, flags, statbuf));
}

namespace {
	bool group_cmp(gid_t against) {
		const gid_t gid = sys_getgid();
		if (gid == against)
			return true;
#ifndef MLIBC_BUILDING_RTLD
		int ret = 0;
		__ensure(sys_getgroups(0, nullptr, &ret) == 0);
		gid_t *gids = (gid_t *)calloc(ret, sizeof(gid_t));
		const int count = ret;
		__ensure(sys_getgroups(count, gids, &ret) == 0);
		for (int i = 0; i < count; i++) {
			if (gids[i] == against) {
				free(gids);
				return true;
			}
		}
		free(gids);
#endif
		return false;
	}

	int access_common(struct stat *st, int mode) {
		const uid_t uid = sys_getuid();

		mode_t mode_mask = 0;
		if (uid == st->st_uid)
			mode_mask |=
			    (((mode & R_OK) ? 0400 : 0000) | ((mode & W_OK) ? 0200 : 0000)
		    	 | ((mode & X_OK) ? 0100 : 0000));
		else if (group_cmp(st->st_gid))
			mode_mask |=
		    	(((mode & R_OK) ? 0040 : 0000) | ((mode & W_OK) ? 0020 : 0000)
			     | ((mode & X_OK) ? 0010 : 0000));
		else
			mode_mask |=
		    	(((mode & R_OK) ? 0004 : 0000) | ((mode & W_OK) ? 0002 : 0000)
			     | ((mode & X_OK) ? 0001 : 0000));

		return (((st->st_mode & 0777) & mode_mask) == mode_mask) ? 0 : EACCES;
	}
} /* anon namespace */

int sys_access(const char *path, int mode) {
	mode &= 7;

	struct stat st = {};
	int ec = sys_stat(fsfd_target::path, -1, path, 0, &st);
	if (ec != 0)
		return ec;
	else if (mode == F_OK)
		return 0;

	return access_common(&st, mode);
}

int sys_faccessat(int dirfd, const char *pathname, int mode, int flags) {
	mode &= 7;

	fsfd_target fsfdt = fsfd_target::none;
	if (dirfd == AT_FDCWD)
		fsfdt = fsfd_target::path;
	else
		fsfdt = fsfd_target::fd;

	struct stat st = {};
	int ec = sys_stat(fsfdt, dirfd, pathname, flags, &st);
	if (ec != 0)
		return ec;
	else if (mode == F_OK)
		return 0;

	return access_common(&st, mode);
}

int sys_readlinkat(int dirfd, const char *path, void *buffer, size_t size, ssize_t *read) {
	return parse_file_status(
	    (obos_status)syscall5(Sys_ReadLinkAt, dirfd, path, buffer, size, read)
	);
}
int sys_readlink(const char *path, void *buffer, size_t size, ssize_t *read) {
	return parse_file_status(
	    (obos_status)syscall5(Sys_ReadLinkAt, AT_FDCWD, path, buffer, size, read)
	);
}

int sys_mkdir(const char *path, mode_t mode) {
	return parse_file_status((obos_status)syscall2(Sys_Mkdir, path, mode));
}
int sys_mkdirat(int dirfd, const char *path, mode_t mode) {
	return parse_file_status((obos_status)syscall3(Sys_MkdirAt, (handle)dirfd, path, mode));
}

int sys_unlinkat(int dirfd, const char *path, int flags) {
	return parse_file_status((obos_status)syscall3(Sys_UnlinkAt, (handle)dirfd, path, flags));
}
int sys_rmdir(const char *path) { return sys_unlinkat(AT_FDCWD, path, AT_REMOVEDIR); }

typedef struct drv_fs_info {
	size_t fsBlockSize;
	size_t freeBlocks; // in units of 'fsBlockSize'

	size_t partBlockSize;
	size_t szFs; // in units of 'partBlockSize'

	size_t fileCount;
	size_t availableFiles; // the count of files that can be made until the partition cannot hold
	                       // anymore

	size_t nameMax;

	uint32_t flags;
} drv_fs_info;

enum {
	FS_FLAGS_NOEXEC = 1 << 0,
	FS_FLAGS_RDONLY = 1 << 1,
};

int sys_statvfs(const char *path, struct statvfs *out) {
	obos_status status = OBOS_STATUS_SUCCESS;
	handle hnd = (handle)syscall2(Sys_OpenDir, path, &status);
	if (status != OBOS_STATUS_SUCCESS)
		return parse_file_status(status);
	int ec = sys_fstatvfs(hnd, out);
	syscall1(Sys_HandleClose, hnd);
	return ec;
}

int sys_fstatvfs(int fd, struct statvfs *out) {
	handle hnd = (handle)fd;
	if (HANDLE_TYPE(hnd) != 2 /* HANDLE_TYPE_DIRENT */)
		return EINVAL;
	if (!out)
		return EINVAL;

	drv_fs_info info = {};
	obos_status status = (obos_status)syscall2(Sys_StatFSInfo, hnd, &info);
	if (status != OBOS_STATUS_SUCCESS)
		return parse_file_status(status);

	out->f_bsize = info.fsBlockSize;
	out->f_bfree = info.freeBlocks;
	out->f_bavail = out->f_bfree;

	out->f_frsize = info.partBlockSize;
	out->f_blocks = info.szFs;

	out->f_files = info.fileCount;
	out->f_ffree = info.availableFiles;
	out->f_favail = out->f_ffree;

	out->f_fsid = 0;
	out->f_namemax = info.nameMax;
	out->f_flag = ST_NOSUID;

	if (info.flags & FS_FLAGS_RDONLY)
		out->f_flag |= ST_RDONLY;
#ifdef ST_NOEXEC
	if (info.flags & FS_FLAGS_NOEXEC)
		out->f_flag |= ST_NOEXEC;
#endif

	return 0;
}

int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
	return parse_file_status((obos_status)syscall4(Sys_FdRead, fd, buf, count, bytes_read));
}

int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
	return parse_file_status((obos_status)syscall4(Sys_FdWrite, fd, buf, count, bytes_written));
}

int sys_pread(int fd, void *buf, size_t n, off_t off, ssize_t *bytes_read) {
	return parse_file_status((obos_status)syscall5(Sys_FdPRead, fd, buf, n, bytes_read, off));
}
int sys_pwrite(int fd, const void *buf, size_t n, off_t off, ssize_t *bytes_written) {
	return parse_file_status((obos_status)syscall5(Sys_FdPWrite, fd, buf, n, bytes_written, off));
}

int sys_seek(int fd, off_t offset, int whence, off_t *new_offset) {
	/*if (offset == 0 && whence == SEEK_CUR)
	{
	    *new_offset = syscall1(Sys_FdTellOff, fd);
	    if (*new_offset == -1)
	        return EBADF;
	}*/
	if (whence > SEEK_DATA)
		return ENOSYS;
	obos_status st = (obos_status)syscall3(Sys_FdSeek, fd, offset, whence);
	int ec = parse_file_status(st);
	if (new_offset) {
		*new_offset = syscall1(Sys_FdTellOff, fd);
		if (*new_offset == -1)
			return EBADF;
	}
	return ec;
}

int sys_close(int fd) { return parse_file_status((obos_status)syscall1(Sys_HandleClose, fd)); }

void sys_sync() { syscall0(Sys_Sync); }

int sys_fsync(int fd) { return parse_file_status((obos_status)syscall1(Sys_FdFlush, fd)); }

int sys_fdatasync(int fd) { return sys_fsync(fd); }

int sys_anon_allocate(size_t sz, void **pointer) {
	struct {
		uint32_t prot;
		uint32_t flags;
		handle file;
		uintptr_t offset;
	} extra_args = {0, 0, HANDLE_INVALID, 0};
	*pointer =
	    (void *)syscall5(Sys_VirtualMemoryAlloc, HANDLE_CURRENT, NULL, sz, &extra_args, NULL);
	return 0;
}

int sys_anon_free(void *blk, size_t sz) {
	return syscall3(Sys_VirtualMemoryFree, HANDLE_CURRENT, blk, sz) == 0 ? 0 : EINVAL;
}

int sys_vm_map(void *hint, size_t size, int prot, int flags, int fd, off_t offset, void **window) {
	unsigned real_flags = 0;
	unsigned prot_flags = 0;
	if ((prot & PROT_READ) && (~prot & PROT_WRITE))
		prot_flags |= OBOS_PROTECTION_READ_ONLY;
	if (prot & PROT_EXEC)
		prot_flags |= OBOS_PROTECTION_EXECUTABLE;

	if (flags & MAP_PRIVATE)
		real_flags |= VMA_FLAGS_PRIVATE;
	// if (flags & MAP_ANON)
	//     real_flags = 0;
#if __MLIBC_LINUX_OPTION
	if (flags & MAP_STACK)
		real_flags |= VMA_FLAGS_GUARD_PAGE;
	if ((~flags & MAP_FIXED) && (~flags & MAP_FIXED_NOREPLACE))
		real_flags |= VMA_FLAGS_HINT;
	if (flags & MAP_POPULATE)
		real_flags |= VMA_FLAGS_PREFAULT;
	if (flags & MAP_HUGETLB)
		real_flags |= VMA_FLAGS_HUGE_PAGE;
#else
	if (~flags & MAP_FIXED)
		real_flags |= VMA_FLAGS_HINT;
#endif

	struct {
		uint32_t prot;
		uint32_t flags;
		handle file;
		uintptr_t offset;
	} extra_args = {prot_flags, real_flags, (handle)fd, (uintptr_t)offset};

	obos_status status = OBOS_STATUS_SUCCESS;

	*window =
	    (void *)syscall5(Sys_VirtualMemoryAlloc, HANDLE_CURRENT, hint, size, &extra_args, &status);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_IN_USE:
			return EEXIST;
		case OBOS_STATUS_UNINITIALIZED:
			return EACCES;
		case OBOS_STATUS_NOT_ENOUGH_MEMORY:
			return ENOMEM;
		default:
			sys_libc_log("unknown error code from Sys_VirtualMemoryAlloc, returning EINVAL\n");
			return EINVAL;
	}
}

int sys_mlock(const void *addr, size_t length) {
	obos_status status = (obos_status)syscall3(Sys_VirtualMemoryLock, HANDLE_CURRENT, addr, length);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_IN_USE:
			return EEXIST;
		case OBOS_STATUS_UNINITIALIZED:
			return EACCES;
		case OBOS_STATUS_NOT_ENOUGH_MEMORY:
			return ENOMEM;
		case OBOS_STATUS_NOT_FOUND:
			return EINVAL;
		default:
			sys_libc_log("unknown error code from Sys_VirtualMemoryLock, returning EINVAL\n");
			return EINVAL;
	}
}

int sys_munlock(const void *addr, size_t length)
{
	obos_status status = (obos_status)syscall3(Sys_VirtualMemoryUnlock, HANDLE_CURRENT, addr, length);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_IN_USE:
			return EEXIST;
		case OBOS_STATUS_UNINITIALIZED:
			return EACCES;
		case OBOS_STATUS_NOT_ENOUGH_MEMORY:
			return ENOMEM;
		case OBOS_STATUS_NOT_FOUND:
			return EINVAL;
		default:
			sys_libc_log("unknown error code from Sys_VirtualMemoryUnlock, returning EINVAL\n");
			return EINVAL;
	}
}

int sys_vm_protect(void *pointer, size_t size, int prot) {
	unsigned prot_flags = 0;
	if ((prot & PROT_READ) && (~prot & PROT_WRITE))
		prot_flags |= OBOS_PROTECTION_READ_ONLY;
	if (prot & PROT_EXEC)
		prot_flags |= OBOS_PROTECTION_EXECUTABLE;

	obos_status status =
	    (obos_status)syscall4(Sys_VirtualMemoryProtect, HANDLE_CURRENT, pointer, size, prot_flags);
	switch (status) {
		case OBOS_STATUS_SUCCESS:
			return 0;
		case OBOS_STATUS_INVALID_ARGUMENT:
			return EINVAL;
		case OBOS_STATUS_IN_USE:
			return EEXIST;
		case OBOS_STATUS_UNINITIALIZED:
			return EACCES;
		case OBOS_STATUS_NOT_ENOUGH_MEMORY:
			return ENOMEM;
		case OBOS_STATUS_NOT_FOUND:
			return EINVAL;
		default:
			sys_libc_log("unknown error code from Sys_VirtualMemoryProtect, returning EINVAL\n");
			return EINVAL;
	}
}

int sys_vm_unmap(void *pointer, size_t size) { return sys_anon_free(pointer, size); }

int sys_sysconf(int num, long *ret) {
	return syscall2(Sys_SysConf, num, ret) == OBOS_STATUS_SUCCESS ? 0 : ENOSYS;
}

void sys_yield() { syscall0(Sys_Yield); }

int sys_sleep(time_t *secs, long *nanos) {
	long ms = *secs * 1000;
	if (!ms)
		ms = *nanos / 1000000;
	if (!ms)
		return EINVAL;
	syscall2(Sys_SleepMS, ms, nullptr);
	return 0;
}

#if __MLIBC_BSD_OPTION
int sys_brk(void **out) {
	(void)out;
	return ENOMEM;
}
#endif

struct sys_socket_io_params {
	union {
		struct sockaddr *sock_addr;
		const struct sockaddr *csock_addr;
	};
	// Untouched in sendto, modified in recvfrom
	size_t addr_length;
	union {
		size_t nRead;
		size_t nWritten;
	};
};

int sys_socket(int family, int type, int protocol, int *fd) {
	*fd = syscall0(Sys_FdAlloc);
	int ec = parse_file_status((obos_status)syscall4(Sys_Socket, *fd, family, type, protocol));
	if (ec)
		syscall1(Sys_HandleClose, *fd);
	return ec;
}

ssize_t sys_sendto(
    int fd,
    const void *buffer,
    size_t size,
    int flags,
    const struct sockaddr *sock_addr,
    socklen_t addr_length,
    ssize_t *length
) {
	struct sys_socket_io_params params = {
	    .csock_addr = sock_addr, .addr_length = addr_length, .nWritten = 0
	};
	int ec = parse_file_status((obos_status)syscall5(Sys_SendTo, fd, buffer, size, flags, &params));
	if (length)
		*length = params.nWritten;
	return ec;
}

ssize_t sys_recvfrom(
    int fd,
    void *buffer,
    size_t size,
    int flags,
    struct sockaddr *sock_addr,
    socklen_t *addr_length,
    ssize_t *length
) {
	struct sys_socket_io_params params = {
	    .sock_addr = sock_addr, .addr_length = addr_length ? *addr_length : 0, .nRead = 0
	};
	int ec =
	    parse_file_status((obos_status)syscall5(Sys_RecvFrom, fd, buffer, size, flags, &params));
	if (length)
		*length = params.nRead;
	if (addr_length)
		*addr_length = params.addr_length;
	return ec;
}

int sys_listen(int fd, int backlog) {
	return parse_file_status((obos_status)syscall2(Sys_Listen, fd, backlog));
}

int sys_accept(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	*newfd = syscall0(Sys_FdAlloc);
	size_t saddr_klength = addr_length ? *addr_length : 0;
	int ec = parse_file_status((obos_status)syscall5(
	    Sys_Accept, fd, *newfd, addr_ptr, addr_length ? &saddr_klength : nullptr, flags
	));
	if (addr_length)
		*addr_length = saddr_klength;
	if (ec)
		syscall1(Sys_HandleClose, *newfd);
	return ec;
}

int sys_bind(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return parse_file_status((obos_status)syscall3(Sys_Bind, fd, addr_ptr, addr_length));
}

int sys_connect(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	return parse_file_status((obos_status)syscall3(Sys_Connect, fd, addr_ptr, addr_length));
}

int sys_sockname(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	size_t actual_length_s = 0;
	int ec = parse_file_status(
	    (obos_status)syscall4(Sys_SockName, fd, addr_ptr, max_addr_length, &actual_length_s)
	);
	if (!ec && actual_length)
		*actual_length = actual_length_s;
	return ec;
}

int sys_peername(
    int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length, socklen_t *actual_length
) {
	size_t actual_length_s = 0;
	int ec = parse_file_status(
	    (obos_status)syscall4(Sys_PeerName, fd, addr_ptr, max_addr_length, &actual_length_s)
	);
	if (!ec && actual_length)
		*actual_length = actual_length_s;
	return ec;
}

int
sys_getsockopt(int fd, int layer, int number, void *__restrict buffer, socklen_t *__restrict size) {
	size_t sz = *size;
	int ec =
	    parse_file_status((obos_status)syscall5(Sys_GetSockOpt, fd, layer, number, buffer, &sz));
	if (!ec)
		*size = sz;
	return ec;
}

int sys_setsockopt(int fd, int layer, int number, const void *buffer, socklen_t size) {
	return parse_file_status(
	    (obos_status)syscall5(Sys_SetSockOpt, fd, layer, number, buffer, size)
	);
}

int sys_shutdown(int sockfd, int how) {
	return parse_file_status((obos_status)syscall2(Sys_ShutdownSocket, sockfd, how));
}

int sys_gethostname(char *buffer, size_t bufsize) {
	obos_status st = (obos_status)syscall2(Sys_GetHostname, buffer, bufsize);
	switch (st) {
		case OBOS_STATUS_INVALID_ARGUMENT:
			return ENAMETOOLONG;
		case OBOS_STATUS_SUCCESS:
			return 0;
		default:
			return parse_file_status(st);
	}
}

int sys_sethostname(const char *buffer, size_t bufsize) {
	return parse_file_status((obos_status)syscall2(Sys_SetHostname, buffer, bufsize));
}

#if defined(__x86_64__)
typedef __uint128_t thread_affinity;
#elif defined(__m68k__)
typedef uint32_t thread_affinity;
#else
#error typedef thread_affinity for the new port!
#endif

int sys_setaffinity(pid_t pid, size_t cpusetsize, const cpu_set_t *mask) {
	return sys_setthreadaffinity(pid, cpusetsize, mask);
}
int sys_setthreadaffinity(pid_t tid, size_t cpusetsize, const cpu_set_t *mask) {
	thread_affinity affinity = 0;
	memcpy(&affinity, mask, frg::min(cpusetsize, sizeof(thread_affinity)));
	handle thr = syscall1(Sys_ThreadOpen, tid);
	int ec = parse_file_status((obos_status)syscall3(Sys_ThreadAffinity, thr, &affinity, nullptr));
	syscall1(Sys_HandleClose, thr);
	return ec;
}

int sys_getaffinity(pid_t pid, size_t cpusetsize, cpu_set_t *mask) {
	return sys_getthreadaffinity(pid, cpusetsize, mask);
}
int sys_getthreadaffinity(pid_t tid, size_t cpusetsize, cpu_set_t *mask) {
	thread_affinity res = 0;
	handle thr = syscall1(Sys_ThreadOpen, tid);
	int ec = parse_file_status((obos_status)syscall3(Sys_ThreadAffinity, thr, nullptr, &res));
	if (ec != 0)
		return ec;
	memcpy(mask, &res, frg::min(cpusetsize, sizeof(res)));
	return 0;
}

int sys_inet_configured(bool *ipv4, bool *ipv6) {
	if (ipv4)
		*ipv4 = true;
	if (ipv6)
		*ipv6 = false;
	return 0;
}

int sys_getentropy(void *buffer, size_t max_size) {
	handle hnd = syscall0(Sys_FdAlloc);

	// Open FD for reading.
	obos_status status = (obos_status)syscall3(Sys_FdOpen, hnd, "/dev/entropy", (1 << 0));
	if (obos_is_error(status)) {
		syscall1(Sys_HandleClose, hnd);
		return parse_file_status(status);
	}

	status = (obos_status)syscall4(Sys_FdRead, hnd, buffer, max_size, nullptr);
	syscall1(Sys_HandleClose, hnd);

	return parse_file_status(status);
}

} // namespace mlibc

namespace {
	int interpret_signal_status(obos_status st) {
		switch (st) {
			case OBOS_STATUS_SUCCESS:
				return 0;
			case OBOS_STATUS_INVALID_ARGUMENT:
				return EINVAL;
			case OBOS_STATUS_NOT_FOUND:
				return ESRCH;
			case OBOS_STATUS_ACCESS_DENIED:
				return EPERM;
			default:
				return ENOSYS;
		}
	}
	int parse_pgid_status(obos_status status) {
		switch (status) {
			case OBOS_STATUS_SUCCESS:
				return 0;
			case OBOS_STATUS_INVALID_ARGUMENT:
				return EINVAL;
			case OBOS_STATUS_NOT_FOUND:
				return ESRCH;
			case OBOS_STATUS_ACCESS_DENIED:
				return EPERM;
			case OBOS_STATUS_INVALID_OPERATION:
				return EIO;
			default:
				return parse_file_status(status);
		}
	}
	int parse_file_status(obos_status status) {
		switch (status) {
			case OBOS_STATUS_SUCCESS:
				return 0;
			case OBOS_STATUS_NOT_FOUND:
				return ENOENT;
			case OBOS_STATUS_INVALID_ARGUMENT:
				return EINVAL;
			case OBOS_STATUS_INTERNAL_ERROR:
				return EIO;
			case OBOS_STATUS_PAGE_FAULT:
				return EFAULT;
			case OBOS_STATUS_NOT_A_FILE:
				return EISDIR;
			case OBOS_STATUS_UNINITIALIZED:
				return EBADF;
			case OBOS_STATUS_EOF:
				return 0;
			case OBOS_STATUS_IN_USE:
				return EEXIST;
			case OBOS_STATUS_ACCESS_DENIED:
				return EACCES;
			case OBOS_STATUS_NO_SYSCALL:
				return ENOSYS;
			case OBOS_STATUS_NOT_ENOUGH_MEMORY:
				return ENOSPC;
			case OBOS_STATUS_ALREADY_INITIALIZED:
				return EEXIST;
			case OBOS_STATUS_UNIMPLEMENTED:
				return ENOSYS;
			case OBOS_STATUS_PIPE_CLOSED:
				return EPIPE;
			case OBOS_STATUS_ALREADY_MOUNTED:
				return EBUSY;
			case OBOS_STATUS_NO_SPACE:
				return ERANGE;
			case OBOS_STATUS_NOT_A_TTY:
				return ENOTTY;
			case OBOS_STATUS_INVALID_IOCTL:
				return EINVAL;
			case OBOS_STATUS_RETRY:
			case OBOS_STATUS_TIMED_OUT:
				return EAGAIN;
			case OBOS_STATUS_ABORTED:
				return EINTR;
			case OBOS_STATUS_INVALID_OPERATION:
				return EIO;
			case OBOS_STATUS_WOULD_BLOCK:
				return EBUSY;
			case OBOS_STATUS_PORT_IN_USE:
				return EADDRINUSE;
			case OBOS_STATUS_ADDRESS_IN_USE:
				return EADDRINUSE;
			case OBOS_STATUS_ADDRESS_NOT_AVAILABLE:
				return EADDRNOTAVAIL;
			case OBOS_STATUS_NO_ROUTE_TO_HOST:
				return EHOSTUNREACH;
			case OBOS_STATUS_NETWORK_UNREACHABLE:
				return ENETUNREACH;
			case OBOS_STATUS_MESSAGE_TOO_BIG:
				return EMSGSIZE;
			case OBOS_STATUS_CONNECTION_REFUSED:
				return ECONNREFUSED;
			case OBOS_STATUS_SEEK_UNALLOWED:
				return ESPIPE;
			default:
				mlibc::sys_libc_log("Unknown obos status code returned from a VFS syscall, returning EIO\n");
				return EIO;
		}
	}
}
