
#include <string.h>

#include <bits/ensure.h>
#include <frg/eternal.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/internal-sysdeps.hpp>

// --------------------------------------------------------
// Globals
// --------------------------------------------------------

MemoryAllocator &getAllocator() {
	// use frg::eternal to prevent a call to __cxa_atexit().
	// this is necessary because __cxa_atexit() call this function.
	static frg::eternal<VirtualAllocator> virtualAllocator;
	static frg::eternal<MemoryPool> heap{virtualAllocator.get()};
	static frg::eternal<MemoryAllocator> singleton{&heap.get()};
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

