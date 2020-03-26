#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <mlibc/sysdeps.hpp>
#include <bits/ensure.h>
#include <frg/slab.hpp>

struct AllocatorLock {
	AllocatorLock()
	: _futex{0} { }

	AllocatorLock(const AllocatorLock &) = delete;

	AllocatorLock &operator= (const AllocatorLock &) = delete;

	static constexpr inline uint32_t waitersBit = (1 << 31);

	void lock() {

		unsigned int expected = 0;
		while(true) {
			if(!expected) {
				// Try to take the mutex here.
				if(__atomic_compare_exchange_n(&_futex, &expected, 1,
							false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
					return;
				}
			}else{
				if(expected & waitersBit) {
					if(int e = mlibc::sys_futex_wait(&_futex, expected); e)
						__ensure(!"sys_futex_wait() failed");

					expected = 0;
				}else{
					unsigned int desired = expected | waitersBit;
					if(__atomic_compare_exchange_n(&_futex, &expected, desired,
							false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
						expected = desired;
				}
			}
		}
	}

	void unlock() {
		if (__atomic_exchange_n(&_futex, 0, __ATOMIC_RELEASE) & waitersBit)
			if(int e = mlibc::sys_futex_wake(&_futex); e)
				__ensure(!"sys_futex_wake() failed");
	}

private:
	int _futex;
};

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_pool<VirtualAllocator, AllocatorLock> MemoryPool;

typedef frg::slab_allocator<VirtualAllocator, AllocatorLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#endif // MLIBC_FRIGG_ALLOC
