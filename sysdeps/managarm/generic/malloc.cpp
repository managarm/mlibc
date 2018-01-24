
#include <stdlib.h>
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/cxx-support.hpp>
#include <mlibc/frigg-alloc.hpp>

#include <hel.h>
#include <hel-syscalls.h>

// --------------------------------------------------------
// Globals
// --------------------------------------------------------

MemoryAllocator &getAllocator() {
	// use frigg::Eternal to prevent a call to __cxa_atexit().
	// this is necessary because __cxa_atexit() call this function.
	static frigg::Eternal<VirtualAllocator> virtualAllocator;
	static frigg::Eternal<MemoryAllocator> singleton(virtualAllocator.get());
	return singleton.get();
}

// --------------------------------------------------------
// VirtualAllocator
// --------------------------------------------------------

uintptr_t VirtualAllocator::map(size_t length) {
	__ensure((length % 0x1000) == 0);

	HelHandle memory;
	void *actual_ptr;
	HEL_CHECK(helAllocateMemory(length, 0, &memory));
	HEL_CHECK(helMapMemory(memory, kHelNullHandle, nullptr, 0, length,
			kHelMapProtRead | kHelMapProtWrite | kHelMapCopyOnWriteAtFork, &actual_ptr));
	HEL_CHECK(helCloseDescriptor(memory));
	return (uintptr_t)actual_ptr;
}

void VirtualAllocator::unmap(uintptr_t address, size_t length) {
	HEL_CHECK(helUnmapMemory(kHelNullHandle, (void *)address, length));
}

void free(void *pointer) {
	getAllocator().free(pointer);
}

void *malloc(size_t size) {
	return getAllocator().allocate(size);
}

void *realloc(void *pointer, size_t size) {
	return getAllocator().realloc(pointer, size);
}

