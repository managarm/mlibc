#include <mlibc/thread-entry.hpp>
#include <mlibc/sysdeps.hpp>
#include <bits/ensure.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg, void *tcb) {
	if(mlibc::sys_tcb_set(tcb))
		__ensure(!"sys_tcb_set() failed");

	void (*func)(void *) = reinterpret_cast<void (*)(void *)>(entry);
	func(user_arg);

	__builtin_trap();
}

namespace mlibc {

void *prepare_stack(void *entry, void *user_arg, void *tcb) {
	uintptr_t *sp = reinterpret_cast<uintptr_t *>(reinterpret_cast<uintptr_t>(
			mmap(nullptr, 0x1000000, 
				PROT_READ | PROT_WRITE, 
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
			) + 0x1000000);

	*--sp = reinterpret_cast<uintptr_t>(tcb);
	*--sp = reinterpret_cast<uintptr_t>(user_arg);
	*--sp = reinterpret_cast<uintptr_t>(entry);
	return sp;
}

} //namespace mlibc
