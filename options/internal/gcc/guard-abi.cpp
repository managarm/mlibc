
#include <stddef.h>
#include <stdint.h>

#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tid.hpp>

namespace {

// Itanium ABI static initialization guard.
struct Guard {
	static constexpr uint32_t waitersBit = 0x80000000;
	static constexpr uint32_t ownerMask = 0x3FFFFFFF;

	void lock() {
		uint32_t tid = mlibc::this_tid();
		uint32_t expected = 0;

		while (true) {
			if (!expected) {
				if (__atomic_compare_exchange_n(
				        &mutex, &expected, tid, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE
				    ))
					return;
			} else {
				if ((expected & ownerMask) == tid)
					mlibc::panicLogger()
					    << "mlibc: __cxa_guard_acquire deadlock detected!" << frg::endlog;

				if (expected & waitersBit) {
					int e = mlibc::sysdep<FutexWait>((int *)&mutex, expected, nullptr);
					if (e && e != EAGAIN && e != EINTR)
						mlibc::panicLogger()
						    << "sys_futex_wait() failed with error code " << e << frg::endlog;
					expected = 0;
				} else {
					uint32_t desired = expected | waitersBit;
					if (__atomic_compare_exchange_n(
					        &mutex, &expected, desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED
					    ))
						expected = desired;
				}
			}
		}
	}

	void unlock() {
		uint32_t state = __atomic_exchange_n(&mutex, 0, __ATOMIC_RELEASE);
		__ensure((state & ownerMask) == mlibc::this_tid());
		if(state & waitersBit)
			mlibc::sysdep<FutexWake>((int *)&mutex, true);
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

} // namespace

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

