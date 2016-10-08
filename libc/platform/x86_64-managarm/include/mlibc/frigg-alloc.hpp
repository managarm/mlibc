
#ifndef MLIBC_FRIGG_ALLOC
#define MLIBC_FRIGG_ALLOC

#include <frigg/initializer.hpp>
#include <frigg/memory.hpp>

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frigg::SlabAllocator<VirtualAllocator, frigg::TicketLock> MemoryAllocator;

extern VirtualAllocator virtualAllocator;
extern frigg::LazyInitializer<MemoryAllocator> memoryAllocator;

#endif // MLIBC_FRIGG_ALLOC

