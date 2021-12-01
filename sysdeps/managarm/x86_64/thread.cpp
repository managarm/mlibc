#include <mlibc/thread-entry.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <bits/ensure.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg, Tcb *tcb) {
	// Wait until our parent sets up the TID.
	while(!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED))
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);

	if(mlibc::sys_tcb_set(tcb))
		__ensure(!"sys_tcb_set() failed");

	void *(*func)(void *) = reinterpret_cast<void *(*)(void *)>(entry);
	auto result = func(user_arg);

	auto self = reinterpret_cast<Tcb *>(tcb);

	self->returnValue = result;
	__atomic_store_n(&self->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&self->didExit);

	mlibc::sys_thread_exit();
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
