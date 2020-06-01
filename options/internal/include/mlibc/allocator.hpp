#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <mlibc/lock.hpp>
#include <bits/ensure.h>
#include <frg/slab.hpp>

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_pool<VirtualAllocator, FutexLock> MemoryPool;

typedef frg::slab_allocator<VirtualAllocator, FutexLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#endif // MLIBC_FRIGG_ALLOC
