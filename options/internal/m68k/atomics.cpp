#include <frg/hash.hpp>
#include <mlibc/lock.hpp>
#include <stdbool.h>
#include <stdint.h>

namespace {

constexpr size_t num_locks = 32;

constinit FutexLock atomic_locks[num_locks];

inline FutexLock &get_lock(const volatile void *ptr) {
	// Mix the address: its low bits are zero for aligned objects and would collide.
	auto hash = frg::mix_bits(reinterpret_cast<uintptr_t>(ptr));
	return atomic_locks[hash & (num_locks - 1)];
}

// m68k aligns uint64_t to two bytes, hence we can dereference *ptr instead of memcpy()ing it.
using aliased_uint64_t [[gnu::may_alias]] = uint64_t;

inline volatile aliased_uint64_t *access(volatile void *ptr) {
	return reinterpret_cast<volatile aliased_uint64_t *>(ptr);
}

inline const volatile aliased_uint64_t *access(const volatile void *ptr) {
	return reinterpret_cast<const volatile aliased_uint64_t *>(ptr);
}

} // namespace

extern "C" {

// Like libatomic, we implement these under internal names and attach the public names with asm labels.
// This is necessary because the ABI of the functions (e.g., bool weak arguments)
// differs among the builtins and the libatomic functions which makes GCC complain otherwise.

uint64_t mlibc_atomic_load_8(
	const volatile void *ptr, int model
) asm("__atomic_load_8");

void mlibc_atomic_store_8(
	volatile void *ptr, uint64_t val, int model
) asm("__atomic_store_8");

uint64_t mlibc_atomic_exchange_8(
	volatile void *ptr, uint64_t val, int model
) asm("__atomic_exchange_8");

bool mlibc_atomic_compare_exchange_8(
    volatile void *ptr,
    void *expected,
    uint64_t desired,
    int success_model,
    int failure_model
) asm("__atomic_compare_exchange_8");

uint64_t mlibc_atomic_fetch_add_8(
	volatile void *ptr, uint64_t val, int model
) asm("__atomic_fetch_add_8");

// Actual definitions below.

uint64_t mlibc_atomic_load_8(const volatile void *ptr, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	uint64_t ret = *access(ptr);
	l.unlock();
	return ret;
}

void mlibc_atomic_store_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	*access(ptr) = val;
	l.unlock();
}

uint64_t mlibc_atomic_exchange_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	uint64_t old = *access(ptr);
	*access(ptr) = val;
	l.unlock();
	return old;
}

bool mlibc_atomic_compare_exchange_8(
    volatile void *ptr,
    void *expected,
    uint64_t desired,
    int success_model [[maybe_unused]],
    int failure_model [[maybe_unused]]
) {
	auto &l = get_lock(ptr);
	bool ret = false;
	l.lock();
	uint64_t old = *access(ptr);
	if (old == *access(expected)) {
		*access(ptr) = desired;
		ret = true;
	} else {
		*access(expected) = old;
	}
	l.unlock();
	return ret;
}

uint64_t mlibc_atomic_fetch_add_8(volatile void *ptr, uint64_t val, int model [[maybe_unused]]) {
	auto &l = get_lock(ptr);
	l.lock();
	uint64_t old = *access(ptr);
	*access(ptr) = old + val;
	l.unlock();
	return old;
}

}
