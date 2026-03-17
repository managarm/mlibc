#include "mlibc/tcb.hpp"
#include <abi-bits/errno.h>
#include <bits/ensure.h>
#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <string.h>

#define SYS_EXIT 0
#define SYS_WRITE 1
#define SYS_MMAP 2

// ANCHOR: stub
#define STUB()                                                                                     \
	({                                                                                             \
		__ensure(!"STUB function was called");                                                     \
		__builtin_unreachable();                                                                   \
	})
// ANCHOR_END: stub

namespace mlibc {

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("!!! mlibc panic !!!");
	sysdep<Exit>(-1);
	__builtin_trap();
}

void Sysdeps<LibcLog>::operator()(const char *msg) {
	ssize_t unused;
	sysdep<Write>(2, msg, strlen(msg), &unused);
}

int Sysdeps<Isatty>::operator()(int fd) {
	(void)fd;
	// this returns ENOTTY when it is not a tty, but we do not have a proper implementation
	// so always return that a file is a tty
	return 0;
}

int Sysdeps<Write>::operator()(int fd, void const *buf, size_t size, ssize_t *ret) {
	*ret = syscall(SYS_WRITE, fd, buf, size);
	// this can never fail in the demo os
	return 0;
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	uintptr_t thread_data = reinterpret_cast<uintptr_t>(pointer) + sizeof(Tcb);
	asm volatile("mv tp, %0" ::"r"(thread_data));
	// this can never fail in the demo os
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	auto out = syscall(
	    SYS_MMAP, nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0
	);
	*pointer = (void *)out;
	if (*pointer == MAP_FAILED)
		return ENOMEM; // the syscall does not return a proper errno, so use ENOMEM
	return 0;
}

int Sysdeps<AnonFree>::operator()(void *, unsigned long) {
	return 0;
} // no-op

int Sysdeps<Seek>::operator()(int, off_t, int, off_t *) {
	return ESPIPE; // no proper file implementation, everything is a tty so return ESPIPE
}

void Sysdeps<Exit>::operator()(int status) {
	syscall(SYS_EXIT, status);
	__builtin_unreachable();
}

// ANCHOR: stubbed-sysdep
int Sysdeps<Close>::operator()(int) {
	STUB();
}
// ANCHOR_END: stubbed-sysdep

int Sysdeps<FutexWake>::operator()(int *, bool) {
	STUB();
}
int Sysdeps<FutexWait>::operator()(int *, int, timespec const *) {
	STUB();
}
int Sysdeps<Read>::operator()(int, void *, unsigned long, long *) {
	STUB();
}
int Sysdeps<Open>::operator()(const char *, int, unsigned int, int *) {
	STUB();
}
int Sysdeps<VmMap>::operator()(void *, size_t, int, int, int, off_t, void **) {
	STUB();
}
int Sysdeps<VmUnmap>::operator()(void *, size_t) {
	STUB();
}
int Sysdeps<ClockGet>::operator()(int, time_t *, long *) {
	STUB();
}

} // namespace mlibc
