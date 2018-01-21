
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <hel.h>
#include <hel-syscalls.h>

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

		helLog("__cxa_guard_acquire contention\n", 31);
		__builtin_trap();
	}

	void unlock() {
		__atomic_store_n(&mutex, 0, __ATOMIC_RELEASE);
	}

	// the first byte's meaning is fixed by the ABI.
	// it indicates whether initialization has already been completed.
	uint8_t complete;

	// we use some of the remaining bytes to implement a mutex.
	uint32_t mutex;
};

static_assert(sizeof(Guard) == sizeof(int64_t));

} // namespace { }

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

typedef void (*InitPtr)();

extern InitPtr __CTOR_LIST__ [[ gnu::visibility("hidden") ]];
extern InitPtr __CTOR_END__ [[ gnu::visibility("hidden") ]];

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_ctors() {
	auto it = &__CTOR_LIST__;
	while(it != &__CTOR_END__)
		(*it++)();
}

extern "C" [[ gnu::visibility("hidden") ]] void __mlibc_do_dtors() {
	helLog("mlibc: fini\n", 12);
}

