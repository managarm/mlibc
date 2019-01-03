
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/allocator.hpp>
#include <mlibc/sysdeps.hpp> 

#include <hel.h>
#include <hel-syscalls.h>

namespace mlibc {

int sys_anon_allocate(size_t size, void **pointer) {
	// This implementation is inherently signal-safe.
	__ensure((size % 0x1000) == 0);

	HelHandle memory;
	HEL_CHECK(helAllocateMemory(size, 0, &memory));
	HEL_CHECK(helMapMemory(memory, kHelNullHandle, nullptr, 0, size,
			kHelMapProtRead | kHelMapProtWrite | kHelMapCopyOnWriteAtFork, pointer));
	HEL_CHECK(helCloseDescriptor(memory));
	return 0;
}

int sys_anon_free(void *pointer, size_t size) {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helUnmapMemory(kHelNullHandle, pointer, size));
	return 0;
}

} //namespace mlibc

