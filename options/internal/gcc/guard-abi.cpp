
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <mlibc/debug.hpp>
#include <mlibc/internal-sysdeps.hpp>

namespace {

// Itanium ABI static initialization guard.
struct Guard {
	// bit of the mutex member variable.
	// indicates that the mutex is locked.
	static constexpr int32_t locked = 1;

	void lock() {
		uint32_t v = 0;
		if(__atomic_compare_exchange_n(&mutex, &v, Guard::locked, false,
				__ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
			return;

		mlibc::sys_libc_log("__cxa_guard_acquire contention");
		__builtin_trap();
	}

	void unlock() {
		__atomic_store_n(&mutex, 0, __ATOMIC_RELEASE);
	}

	// the first byte's meaning is fixed by the ABI.
	// it indicates whether initialization has already been completed.
	uint8_t complete;
	// padding to ensure correct alignment on certain platforms.
	uint8_t padding[3];

	// we use some of the remaining bytes to implement a mutex.
	uint32_t mutex;
};

static_assert(sizeof(Guard) == sizeof(int64_t));

} // namespace { }

extern "C" [[ gnu::visibility("hidden") ]] void __cxa_pure_virtual() {
	mlibc::panicLogger() << "mlibc: Pure virtual function called from IP "
			<< (void *)__builtin_return_address(0) << frg::endlog;
}

extern "C" [[ gnu::visibility("hidden") ]] int __cxa_guard_acquire(int64_t *ptr) {
	auto guard = reinterpret_cast<Guard *>(ptr);
	guard->lock();
	// relaxed ordering is sufficient because
	// Guard::complete is only modified while the mutex is held.
	if(__atomic_load_n(&guard->complete, __ATOMIC_RELAXED)) {
		guard->unlock();
		return 0;
	}else{
		return 1;
	}
}

extern "C" [[ gnu::visibility("hidden") ]] void __cxa_guard_release(int64_t *ptr) {
	auto guard = reinterpret_cast<Guard *>(ptr);
	// do a store-release so that compiler generated code can skip calling
	// __cxa_guard_acquire by doing a load-acquire on Guard::complete.
	__atomic_store_n(&guard->complete, 1, __ATOMIC_RELEASE);
	guard->unlock();
}

