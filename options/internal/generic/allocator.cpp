
#include <stdlib.h>
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/cxx-support.hpp>
#include <mlibc/sysdeps.hpp>

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
	void *ptr;
	__ensure(!mlibc::sys_anon_allocate(length, &ptr));
	return (uintptr_t)ptr;
}

void VirtualAllocator::unmap(uintptr_t address, size_t length) {
	__ensure(!mlibc::sys_anon_free((void *)address, length));
}

