
#include <stdlib.h>
#include <string.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>

#pragma GCC visibility push(hidden)

#include <hel.h>
#include <hel-syscalls.h>

// --------------------------------------------------------
// Globals
// --------------------------------------------------------

VirtualAllocator virtualAllocator;
frigg::LazyInitializer<MemoryAllocator> memoryAllocator;

// --------------------------------------------------------
// VirtualAllocator
// --------------------------------------------------------

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

#pragma GCC visibility pop

void __mlibc_initMalloc() {
	memoryAllocator.initialize(virtualAllocator);
}

void free(void *pointer) {
	memoryAllocator->free(pointer);
}

void *malloc(size_t size) {
	// FIXME: initialize malloc from a global library guard constructor
	__ensure(memoryAllocator);
	return memoryAllocator->allocate(size);
}

