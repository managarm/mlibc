
#include <stdlib.h>
#include <string.h>

#include <mlibc/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>

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
	HEL_CHECK(helMapMemory(memory, kHelNullHandle, nullptr, 0, length,
			kHelMapReadWrite | kHelMapCopyOnWriteAtFork, &actual_ptr));
	HEL_CHECK(helCloseDescriptor(memory));
	return (uintptr_t)actual_ptr;
}

void VirtualAllocator::unmap(uintptr_t address, size_t length) {
	HEL_CHECK(helUnmapMemory(kHelNullHandle, (void *)address, length));
}

void __mlibc_initMalloc() {
	memoryAllocator.initialize(virtualAllocator);
}

void free(void *pointer) {
	__ensure(memoryAllocator);
	memoryAllocator->free(pointer);
}

void *malloc(size_t size) {
	__ensure(memoryAllocator);
	return memoryAllocator->allocate(size);
}

void *realloc(void *pointer, size_t size) {
	__ensure(memoryAllocator);
	return memoryAllocator->realloc(pointer, size);
}

