#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

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

int Sysdeps<Read>::operator()(int, void *, size_t, ssize_t *) {
	STUB();
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

int Sysdeps<Write>::operator()(int, const void *, size_t, ssize_t *) {
	STUB();
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

void Sysdeps<Exit>::operator()(int) {
	STUB();
}

} // namespace mlibc
