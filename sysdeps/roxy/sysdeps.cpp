#include <mlibc/all-sysdeps.hpp>
#include <roxy/syscall.h>

#include <bits/ensure.h>
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
