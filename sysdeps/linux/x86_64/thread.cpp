#include <mlibc/thread-entry.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/thread.hpp>
#include <bits/ensure.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stddef.h>

namespace {
	constexpr unsigned int STACK_SIZE = 0x200000;
	constexpr unsigned int GUARD_SIZE = 0x2000;
}

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg) {
	// The linux kernel already sets the TCB in sys_clone().
	auto tcb = mlibc::get_current_tcb();

	// Wait until our parent sets up the TID.
	while(!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED))
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);

	void *(*func)(void *) = reinterpret_cast<void *(*)(void *)>(entry);
	auto result = func(user_arg);

	tcb->returnValue = result;
	__atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&tcb->didExit);

	mlibc::sys_thread_exit();
}

namespace mlibc {

void *prepare_stack(void *entry, void *user_arg) {
	uintptr_t map = reinterpret_cast<uintptr_t>(
			mmap(nullptr, STACK_SIZE + GUARD_SIZE,
				PROT_NONE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
			);
	__ensure(reinterpret_cast<void*>(map) != MAP_FAILED);
	int ret = mprotect(reinterpret_cast<void*>(map + GUARD_SIZE), STACK_SIZE,
			PROT_READ | PROT_WRITE);
	__ensure(!ret);

	auto sp = reinterpret_cast<uintptr_t*>(map + STACK_SIZE + GUARD_SIZE);
	*--sp = reinterpret_cast<uintptr_t>(user_arg);
	*--sp = reinterpret_cast<uintptr_t>(entry);
	return sp;
}
} // namespace mlibc
