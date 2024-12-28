#include <pthread.h>
#include <stdlib.h>
#include <sys/auxv.h>

#include <frg/eternal.hpp>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/elf/startup.h>
#include <mlibc/posix-pipe.hpp>

#include <protocols/posix/data.hpp>
#include <protocols/posix/supercalls.hpp>

extern "C" uintptr_t *__dlapi_entrystack();
extern "C" void __dlapi_enter(uintptr_t *);

// declared in posix-pipe.hpp
thread_local Queue globalQueue;

// TODO: clock tracker page and file table don't need to be thread-local!
thread_local HelHandle __mlibc_posix_lane;
thread_local void *__mlibc_clk_tracker_page;

namespace {
thread_local unsigned __mlibc_gsf_nesting;
thread_local void *__mlibc_cached_thread_page;
thread_local HelHandle *cachedFileTable;

// This construction is a bit weird: Even though the variables above
// are thread_local we still protect their initialization with a pthread_once_t
// (instead of using a C++ constructor).
// We do this in order to able to clear the pthread_once_t after a fork.
thread_local pthread_once_t has_cached_infos = PTHREAD_ONCE_INIT;

void actuallyCacheInfos() {
	posix::ManagarmProcessData data;
	HEL_CHECK(
	    helSyscall1(kHelCallSuper + posix::superGetProcessData, reinterpret_cast<HelWord>(&data))
	);

	__mlibc_posix_lane = data.posixLane;
	__mlibc_cached_thread_page = data.threadPage;
	cachedFileTable = data.fileTable;
	__mlibc_clk_tracker_page = data.clockTrackerPage;
}
} // namespace

SignalGuard::SignalGuard() {
	pthread_once(&has_cached_infos, &actuallyCacheInfos);
	if (!__mlibc_cached_thread_page)
		return;
	auto p = reinterpret_cast<unsigned int *>(__mlibc_cached_thread_page);
	if (!__mlibc_gsf_nesting)
		__atomic_store_n(p, 1, __ATOMIC_RELAXED);
	__mlibc_gsf_nesting++;
}

SignalGuard::~SignalGuard() {
	pthread_once(&has_cached_infos, &actuallyCacheInfos);
	if (!__mlibc_cached_thread_page)
		return;
	auto p = reinterpret_cast<unsigned int *>(__mlibc_cached_thread_page);
	__ensure(__mlibc_gsf_nesting > 0);
	__mlibc_gsf_nesting--;
	if (!__mlibc_gsf_nesting) {
		unsigned int result = __atomic_exchange_n(p, 0, __ATOMIC_RELAXED);
		if (result == 2) {
			HEL_CHECK(helSyscall0(kHelCallSuper + posix::superSigRaise));
		} else {
			__ensure(result == 1);
		}
	}
}

MemoryAllocator &getSysdepsAllocator() {
	// use frg::eternal to prevent a call to __cxa_atexit().
	// this is necessary because __cxa_atexit() call this function.
	static frg::eternal<VirtualAllocator> virtualAllocator;
	static frg::eternal<MemoryPool> heap{virtualAllocator.get()};
	static frg::eternal<MemoryAllocator> singleton{&heap.get()};
	return singleton.get();
}

HelHandle getPosixLane() {
	cacheFileTable();
	return __mlibc_posix_lane;
}

HelHandle *cacheFileTable() {
	// TODO: Make sure that this is signal-safe (it is called e.g. by sys_clock_get()).
	pthread_once(&has_cached_infos, &actuallyCacheInfos);
	return cachedFileTable;
}

HelHandle getHandleForFd(int fd) {
	if (fd >= 512)
		return 0;

	return cacheFileTable()[fd];
}

void clearCachedInfos() { has_cached_infos = PTHREAD_ONCE_INIT; }

extern char **environ;

extern "C" void
__mlibc_entry(uintptr_t *entry_stack, int (*main_fn)(int argc, char *argv[], char *env[])) {
	__dlapi_enter(entry_stack);
	auto result = main_fn(mlibc::entry_stack.argc, mlibc::entry_stack.argv, environ);
	exit(result);
}
