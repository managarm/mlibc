
#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <frigg/atomic.hpp>
#include <frigg/initializer.hpp>
#include <frigg/memory.hpp>
#include <frg/slab.hpp>

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frg::slab_allocator<VirtualAllocator, frigg::TicketLock> MemoryAllocator;

MemoryAllocator &getAllocator();

#endif // MLIBC_FRIGG_ALLOC

