#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

#include <bits/ensure.h>

static int syscall_result(long result, ssize_t *transferred) {
	if(result < 0)
		return static_cast<int>(-result);

	*transferred = result;
	return 0;
}

#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	STUB();
}

void Sysdeps<LibcLog>::operator()(const char *) {
	STUB();
}

int Sysdeps<FutexWait>::operator()(int *, int, const timespec *) {
	STUB();
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	STUB();
}

int Sysdeps<Open>::operator()(const char *, int, mode_t, int *) {
	STUB();
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t count, ssize_t *bytes_read) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_READ, fd, reinterpret_cast<long>(buffer), count), bytes_read
	);
}

int Sysdeps<Close>::operator()(int) {
	STUB();
}

int Sysdeps<ClockGet>::operator()(int, time_t *, long *) {
	STUB();
}

int Sysdeps<Isatty>::operator()(int) {
	STUB();
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t count, ssize_t *bytes_written) {
	return syscall_result(
	    roxy_syscall3(ROXY_SYS_WRITE, fd, reinterpret_cast<long>(buffer), count), bytes_written
	);
}

int Sysdeps<TcbSet>::operator()(void *) {
	STUB();
}

int Sysdeps<AnonAllocate>::operator()(size_t, void **) {
	STUB();
}

int Sysdeps<AnonFree>::operator()(void *, size_t) {
	STUB();
}

int Sysdeps<VmMap>::operator()(void *, size_t, int, int, int, off_t, void **) {
	STUB();
}

int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	STUB();
}

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	STUB();
}

void Sysdeps<Exit>::operator()(int status) {
	roxy_syscall1(ROXY_SYS_EXIT, status);
	__builtin_unreachable();
}

} // namespace mlibc
