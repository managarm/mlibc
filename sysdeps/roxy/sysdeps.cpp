#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

#include <stdint.h>

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

int Sysdeps<Isatty>::operator()(int fd) {
	return syscall_error(roxy_syscall1(ROXY_SYS_ISATTY, fd));
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t count, ssize_t *bytes_written) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_WRITE, fd, reinterpret_cast<long>(buffer), count), bytes_written
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

pid_t Sysdeps<GetPid>::operator()() {
	return static_cast<pid_t>(roxy_syscall0(ROXY_SYS_GETPID));
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

} // namespace mlibc
