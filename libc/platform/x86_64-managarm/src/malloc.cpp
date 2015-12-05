
#include <stdlib.h>
#include <string.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>

#pragma GCC visibility push(hidden)

#include <frigg/initializer.hpp>
#include <frigg/memory.hpp>

#include <hel.h>
#include <hel-syscalls.h>

struct VirtualAllocator {
public:
	uintptr_t map(size_t length);

	void unmap(uintptr_t address, size_t length);
};

typedef frigg::SlabAllocator<VirtualAllocator, frigg::TicketLock> MemoryAllocator;

uintptr_t VirtualAllocator::map(size_t length) {
	assert((length % 0x1000) == 0);

	HelHandle memory;
	void *actual_ptr;
	HEL_CHECK(helAllocateMemory(length, 0, &memory));
	HEL_CHECK(helMapMemory(memory, kHelNullHandle, nullptr, length,
			kHelMapReadWrite, &actual_ptr));
	HEL_CHECK(helCloseDescriptor(memory));
	return (uintptr_t)actual_ptr;
}

void VirtualAllocator::unmap(uintptr_t address, size_t length) {
	HEL_CHECK(helUnmapMemory(kHelNullHandle, (void *)address, length));
}

VirtualAllocator virtualAllocator;
frigg::LazyInitializer<MemoryAllocator> memoryAllocator;

#pragma GCC visibility pop

void __mlibc_initMalloc() {
	memoryAllocator.initialize(virtualAllocator);
}

void *malloc(size_t size) {
	// FIXME: initialize malloc from a global library guard constructor
	if(!memoryAllocator)
		__mlibc_initMalloc();
	return memoryAllocator->allocate(size);
}

