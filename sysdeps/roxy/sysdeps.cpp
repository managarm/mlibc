#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>

static int syscall_error(long result) {
	return result < 0 ? static_cast<int>(-result) : 0;
}

static int syscall_result(long result, ssize_t *transferred) {
	if(result < 0)
		return static_cast<int>(-result);

	*transferred = result;
	return 0;
}

namespace mlibc {

namespace {

constexpr int log_fd = 2;

void write_text(const char *text) {
	size_t length = 0;
	while(text[length])
		length++;

	size_t transferred = 0;
	while(transferred < length) {
		ssize_t written = 0;
		auto error = sysdep<Write>(
		    log_fd,
		    text + transferred,
		    length - transferred,
		    &written
		);
		if(error || written <= 0)
			return;

		transferred += static_cast<size_t>(written);
	}
}

} // namespace

void Sysdeps<LibcPanic>::operator()() {
	write_text("mlibc panic!\n");
	sysdep<Exit>(1);
}

void Sysdeps<LibcLog>::operator()(const char *message) {
	write_text("mlibc: ");
	write_text(message);
	write_text("\n");
}

int Sysdeps<FutexWait>::operator()(int *pointer, int expected, const timespec *timeout) {
	return syscall_error(roxy_syscall3(
	    ROXY_SYS_FUTEX_WAIT,
	    reinterpret_cast<long>(pointer),
	    expected,
	    reinterpret_cast<long>(timeout)
	));
}

int Sysdeps<FutexWake>::operator()(int *pointer, bool all) {
	return syscall_error(roxy_syscall2(
	    ROXY_SYS_FUTEX_WAKE,
	    reinterpret_cast<long>(pointer),
	    all ? UINT32_MAX : 1
	));
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t mode, int *fd) {
	auto result = roxy_syscall3(
	    ROXY_SYS_OPEN,
	    reinterpret_cast<long>(path),
	    flags,
	    mode
	);
	if(result < 0)
		return static_cast<int>(-result);

	*fd = static_cast<int>(result);
	return 0;
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t count, ssize_t *bytes_read) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_READ, fd, reinterpret_cast<long>(buffer), count), bytes_read
	);
}

int Sysdeps<Close>::operator()(int fd) {
	return syscall_error(roxy_syscall1(ROXY_SYS_CLOSE, fd));
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	auto result = roxy_syscall3(
	    ROXY_SYS_POLL,
	    reinterpret_cast<long>(fds),
	    count,
	    timeout
	);
	if(result < 0)
		return static_cast<int>(-result);

	*num_events = static_cast<int>(result);
	return 0;
}

int Sysdeps<Ppoll>::operator()(
	struct pollfd *fds,
	nfds_t count,
	const struct timespec *timeout,
	const sigset_t *signal_mask,
	int *num_events
) {
	auto result = roxy_syscall4(
	    ROXY_SYS_PPOLL,
	    reinterpret_cast<long>(fds),
	    count,
	    reinterpret_cast<long>(timeout),
	    reinterpret_cast<long>(signal_mask)
	);
	if(result < 0)
		return static_cast<int>(-result);

	*num_events = static_cast<int>(result);
	return 0;
}

int Sysdeps<Pselect>::operator()(
	int num_fds,
	fd_set *read_set,
	fd_set *write_set,
	fd_set *except_set,
	const struct timespec *timeout,
	const sigset_t *signal_mask,
	int *num_events
) {
	auto result = roxy_syscall6(
	    ROXY_SYS_PSELECT,
	    num_fds,
	    reinterpret_cast<long>(read_set),
	    reinterpret_cast<long>(write_set),
	    reinterpret_cast<long>(except_set),
	    reinterpret_cast<long>(timeout),
	    reinterpret_cast<long>(signal_mask)
	);
	if(result < 0)
		return static_cast<int>(-result);

	*num_events = static_cast<int>(result);
	return 0;
}

int Sysdeps<ClockGet>::operator()(int clock, time_t *secs, long *nanos) {
	roxy_clock_result result;
	auto error =
	    syscall_error(roxy_syscall2(ROXY_SYS_CLOCK_GET, clock, reinterpret_cast<long>(&result)));
	if (error)
		return error;

	*secs = result.seconds;
	*nanos = result.nanoseconds;
	return 0;
}

int Sysdeps<Sleep>::operator()(time_t *secs, long *nanos) {
	struct timespec request = {
	    .tv_sec = *secs,
	    .tv_nsec = *nanos,
	};

	auto error = syscall_error(
	    roxy_syscall1(ROXY_SYS_SLEEP, reinterpret_cast<long>(&request))
	);
	if(error)
		return error;

	return 0;
}

int Sysdeps<Uname>::operator()(struct utsname *output) {
	return syscall_error(roxy_syscall1(ROXY_SYS_UNAME, reinterpret_cast<long>(output)));
}

int Sysdeps<Isatty>::operator()(int fd) {
	return syscall_error(roxy_syscall1(ROXY_SYS_ISATTY, fd));
}

int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	return syscall_error(
	    roxy_syscall3(ROXY_SYS_TTYNAME, fd, reinterpret_cast<long>(buf), size)
	);
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t count, ssize_t *bytes_written) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_WRITE, fd, reinterpret_cast<long>(buffer), count), bytes_written
	);
}

int Sysdeps<Writev>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_WRITEV, fd, reinterpret_cast<long>(iovs), iovc), bytes_written
	);
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	return syscall_error(roxy_syscall1(ROXY_SYS_TCB_SET, reinterpret_cast<long>(pointer)));
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	auto result = roxy_syscall1(ROXY_SYS_ANON_ALLOCATE, size);
	if(result < 0)
		return static_cast<int>(-result);

	*pointer = reinterpret_cast<void *>(result);
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	return syscall_error(
	    roxy_syscall2(ROXY_SYS_ANON_FREE, reinterpret_cast<long>(pointer), size)
	);
}

int Sysdeps<VmMap>::operator()(
	void *hint,
	size_t size,
	int prot,
	int flags,
	int fd,
	off_t offset,
	void **window
) {
	auto result = roxy_syscall6(
	    ROXY_SYS_VM_MAP,
	    reinterpret_cast<long>(hint),
	    size,
	    prot,
	    flags,
	    fd,
	    offset
	);
	if(result < 0)
		return static_cast<int>(-result);

	*window = reinterpret_cast<void *>(result);
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	return syscall_error(
	    roxy_syscall2(ROXY_SYS_VM_UNMAP, reinterpret_cast<long>(pointer), size)
	);
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int protection) {
	return syscall_error(roxy_syscall3(
	    ROXY_SYS_VM_PROTECT,
	    reinterpret_cast<long>(pointer),
	    size,
	    protection
	));
}

int Sysdeps<Stat>::operator()(
	fsfd_target target,
	int fd,
	const char *path,
	int flags,
	struct stat *output
) {
	roxy_stat_result result;
	auto error = syscall_error(roxy_syscall5(
	    ROXY_SYS_STAT,
	    static_cast<long>(target),
	    fd,
	    reinterpret_cast<long>(path),
	    flags,
	    reinterpret_cast<long>(&result)
	));
	if(error)
		return error;
	if(result.size > INT64_MAX)
		return EOVERFLOW;

	*output = {};
	output->st_ino = result.file_id;
	output->st_mode = result.mode;
	output->st_nlink = result.hard_links;
	output->st_size = result.size;
	output->st_blksize = result.block_size;
	output->st_blocks = result.blocks;
	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto result = roxy_syscall3(ROXY_SYS_SEEK, fd, offset, whence);
	if(result < 0)
		return static_cast<int>(-result);

	*new_offset = result;
	return 0;
}

gid_t Sysdeps<GetGid>::operator()() {
	return static_cast<gid_t>(roxy_syscall0(ROXY_SYS_GETGID));
}

gid_t Sysdeps<GetEgid>::operator()() {
	return static_cast<gid_t>(roxy_syscall0(ROXY_SYS_GETEGID));
}

uid_t Sysdeps<GetUid>::operator()() {
	return static_cast<uid_t>(roxy_syscall0(ROXY_SYS_GETUID));
}

uid_t Sysdeps<GetEuid>::operator()() {
	return static_cast<uid_t>(roxy_syscall0(ROXY_SYS_GETEUID));
}

int Sysdeps<GetResuid>::operator()(uid_t *ruid, uid_t *euid, uid_t *suid) {
	// Roxy has no user model yet; every ID is 0 (root).
	*ruid = 0;
	*euid = 0;
	*suid = 0;
	return 0;
}

int Sysdeps<GetResgid>::operator()(gid_t *rgid, gid_t *egid, gid_t *sgid) {
	// Roxy has no user model yet; every ID is 0 (root).
	*rgid = 0;
	*egid = 0;
	*sgid = 0;
	return 0;
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	auto result = roxy_syscall2(ROXY_SYS_PIPE, reinterpret_cast<long>(fds), flags);
	return syscall_error(result);
}

int Sysdeps<Dup2>::operator()(int oldfd, int flags, int newfd) {
	// The kernel ABI takes (oldfd, newfd, flags); the mlibc tag passes (fd, flags, newfd).
	auto result = roxy_syscall3(ROXY_SYS_DUP2, oldfd, newfd, flags);
	return syscall_error(result);
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	// dup(fd) = fcntl(fd, F_DUPFD, 0): returns the lowest available fd >= 0.
	auto raw = roxy_syscall3(ROXY_SYS_FCNTL, fd, 0 /* F_DUPFD */, 0);
	if(int error = syscall_error(raw); error)
		return error;
	*newfd = static_cast<int>(raw);
	return 0;
}

int Sysdeps<Fcntl>::operator()(int fd, int command, va_list args, int *result) {
	auto argument = va_arg(args, unsigned long);
	auto raw = roxy_syscall3(ROXY_SYS_FCNTL, fd, command, argument);
	if(int error = syscall_error(raw); error)
		return error;

	*result = static_cast<int>(raw);
	return 0;
}

int Sysdeps<Umask>::operator()(mode_t mode, mode_t *old) {
	// The kernel stores the new mask and returns the previous one.
	auto raw = roxy_syscall1(ROXY_SYS_UMASK, mode);
	if(int error = syscall_error(raw); error)
		return error;

	*old = static_cast<mode_t>(raw);
	return 0;
}

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	auto raw = roxy_syscall2(ROXY_SYS_CHMOD, reinterpret_cast<long>(pathname), mode);
	return syscall_error(raw);
}

int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
	auto raw = roxy_syscall2(ROXY_SYS_FCHMOD, fd, mode);
	return syscall_error(raw);
}

int Sysdeps<Access>::operator()(const char *pathname, int mode) {
	auto raw = roxy_syscall2(ROXY_SYS_ACCESS, reinterpret_cast<long>(pathname), mode);
	return syscall_error(raw);
}

int Sysdeps<GetHostname>::operator()(char *buffer, size_t bufsize) {
	const char *hostname = "roxybestgirl";
	size_t length = strlen(hostname);

	if(length >= bufsize)
		return ENAMETOOLONG;

	memcpy(buffer, hostname, length + 1);
	return 0;
}

int Sysdeps<SetUid>::operator()(uid_t uid) {
	// Roxy currently runs every process as root and has no credential state.
	return uid == 0 ? 0 : EPERM;
}

int Sysdeps<SetGid>::operator()(gid_t gid) {
	// Roxy currently runs every process as root and has no credential state.
	return gid == 0 ? 0 : EPERM;
}

int Sysdeps<SetEuid>::operator()(uid_t euid) {
	// Roxy has no credential state; effective uid is always root.
	return euid == 0 ? 0 : EPERM;
}

int Sysdeps<SetEgid>::operator()(gid_t egid) {
	// Roxy has no credential state; effective gid is always root.
	return egid == 0 ? 0 : EPERM;
}

pid_t Sysdeps<GetPid>::operator()() {
	return static_cast<pid_t>(roxy_syscall0(ROXY_SYS_GETPID));
}

pid_t Sysdeps<GetPpid>::operator()() {
	return static_cast<pid_t>(roxy_syscall0(ROXY_SYS_GETPPID));
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	auto result = roxy_syscall1(ROXY_SYS_FORK, 0);
	if(result < 0)
		return static_cast<int>(-result);
	if(result > INT32_MAX)
		return EOVERFLOW;

	*child = static_cast<pid_t>(result);
	return 0;
}

int Sysdeps<Waitpid>::operator()(
	pid_t pid,
	int *status,
	int flags,
	struct rusage *ru,
	pid_t *ret_pid
) {
	auto result = roxy_syscall4(
	    ROXY_SYS_WAITPID,
	    pid,
	    reinterpret_cast<long>(status),
	    flags,
	    reinterpret_cast<long>(ru)
	);
	if(result < 0)
		return static_cast<int>(-result);

	*ret_pid = static_cast<pid_t>(result);
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	return syscall_error(roxy_syscall3(
	    ROXY_SYS_EXECVE,
	    reinterpret_cast<long>(path),
	    reinterpret_cast<long>(argv),
	    reinterpret_cast<long>(envp)
	));
}

void Sysdeps<Exit>::operator()(int status) {
	roxy_syscall1(ROXY_SYS_EXIT, status);
	__builtin_unreachable();
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	return syscall_error(roxy_syscall2(ROXY_SYS_SET_PGID, pid, pgid));
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	auto result = roxy_syscall1(ROXY_SYS_GET_PGID, pid);
	if(result < 0)
		return static_cast<int>(-result);

	*pgid = static_cast<pid_t>(result);
	return 0;
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
	auto result = roxy_syscall0(ROXY_SYS_SET_SID);
	if(result < 0)
		return static_cast<int>(-result);

	*sid = static_cast<pid_t>(result);
	return 0;
}

} // namespace mlibc
