#include <frg/mutex.hpp>
#include <frg/spinlock.hpp>
#include <stdbool.h>
#include <stdint.h>

namespace {

frg::simple_spinlock atomic_locks[32] = {};

inline frg::simple_spinlock &get_lock(const volatile void *ptr) {
	uintptr_t hash = ((uintptr_t)ptr >> 3) & 31;
	return atomic_locks[hash];
}

} // namespace

extern "C" {

// We need this pragma to avoid compiler warnings about differing declarations;
// the compiler has these functions declared with default visibility, which we
// explicitly want to avoid.
#pragma GCC visibility push(hidden)

uint64_t __atomic_load_8(const volatile void *ptr, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	uint64_t ret;
	__builtin_memcpy(&ret, (const void *)ptr, 8);
	l.unlock();
	return ret;
}

void __atomic_store_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	__builtin_memcpy((void *)ptr, &val, 8);
	l.unlock();
}

uint64_t __atomic_exchange_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	uint64_t old;
	l.lock();
	__builtin_memcpy(&old, (const void *)ptr, 8);
	__builtin_memcpy((void *) ptr, &val, 8);
	l.unlock();
	return old;
}

bool __atomic_compare_exchange_8(
    volatile void *ptr,
    void *expected,
    uint64_t desired,
    bool weak [[maybe_unused]],
    int success_model [[maybe_unused]],
    int failure_model [[maybe_unused]]
) {
	auto &l = get_lock(ptr);
	bool ret = false;
	l.lock();
	if (__builtin_memcmp((void *) ptr, expected, 8) == 0) {
		__builtin_memcpy((void *) ptr, &desired, 8);
		ret = true;
	} else {
		__builtin_memcpy(expected, (const void *)ptr, 8);
	}
	l.unlock();
	return ret;
}

uint64_t __atomic_fetch_add_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	uint64_t old, new_val;
	l.lock();
	__builtin_memcpy(&old, (const void *)ptr, 8);
	new_val = old + val;
	__builtin_memcpy((void *) ptr, &new_val, 8);
	l.unlock();
	return old;
}

#pragma GCC visibility pop

}
