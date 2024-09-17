#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <mlibc/lock.hpp>
#include <bits/ensure.h>
#include <frg/slab.hpp>
#include <internal-config.h>

#if !MLIBC_DEBUG_ALLOCATOR

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_pool<VirtualAllocator, FutexLock> MemoryPool;

typedef frg::slab_allocator<VirtualAllocator, FutexLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#else

struct MemoryAllocator {
	void *allocate(size_t size);
	void free(void *ptr);
	void deallocate(void *ptr, size_t size);
	void *reallocate(void *ptr, size_t size);
	size_t get_size(void *ptr);
};

MemoryAllocator &getAllocator();

#endif // !MLIBC_DEBUG_ALLOCATOR

#endif // MLIBC_FRIGG_ALLOC
