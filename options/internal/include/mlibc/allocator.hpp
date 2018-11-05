#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <bits/ensure.h>
#include <frg/slab.hpp>

struct AllocatorLock {
	AllocatorLock()
	: _futex{0} { }

	AllocatorLock(const AllocatorLock &) = delete;
	
	AllocatorLock &operator= (const AllocatorLock &) = delete;

	void lock() {
		if(__atomic_exchange_n(&_futex, 1, __ATOMIC_ACQUIRE))
			__ensure(!"Implement AllocatorLock slow path");
	}

	void unlock() {
		__atomic_store_n(&_futex, 0, __ATOMIC_RELEASE);
	}

private:
	int _futex;
};

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_allocator<VirtualAllocator, AllocatorLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#endif // MLIBC_FRIGG_ALLOC
