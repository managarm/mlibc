
#include <string.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <protocols/posix/supercalls.hpp>

#include <hel-syscalls.h>
#include <hel.h>

namespace mlibc {

int sys_anon_allocate(size_t size, void **pointer) {
	// This implementation is inherently signal-safe.
	__ensure(!(size & 0xFFF));
	HelWord out;
	HEL_CHECK(helSyscall1_1(kHelCallSuper + posix::superAnonAllocate, size, &out));
	*pointer = reinterpret_cast<void *>(out);
	return 0;
}

int sys_anon_free(void *pointer, size_t size) {
	// This implementation is inherently signal-safe.
	HEL_CHECK(helSyscall2(kHelCallSuper + posix::superAnonDeallocate, (HelWord)pointer, size));
	return 0;
}

} // namespace mlibc
