#include <mlibc/thread-entry.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/thread.hpp>
#include <bits/ensure.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg) {
	// The linux kernel already sets the TCB in sys_clone().
	auto tcb = mlibc::get_current_tcb();

	// Wait until our parent sets up the TID.
	while(!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED))
		mlibc::sysdep<FutexWait>(&tcb->tid, 0, nullptr);

	// Enable cancellation once the TCB is up.
	__atomic_fetch_or(&tcb->cancelBits, tcbCancelEnableBit, __ATOMIC_RELAXED);

	tcb->invokeThreadFunc(entry, user_arg);

	mlibc::thread_exit(tcb->returnValue);
}

namespace mlibc {

static constexpr size_t default_stacksize = 0x200000;

int Sysdeps<PrepareStack>::operator()(void **stack, void *entry, void *user_arg, void *tcb, size_t *stack_size, size_t *guard_size, void **stack_base) {
	(void)tcb;
	if (!*stack_size)
		*stack_size = default_stacksize;

	void *map;
	if (*stack) {
		map = *stack;
		*guard_size = 0;
	} else {
		auto mmap_result = mlibc::sysdep<VmMap>(
		    nullptr, *stack_size + *guard_size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, &map
		);

		if (mmap_result || reinterpret_cast<void*>(map) == MAP_FAILED)
			return EAGAIN;
		int ret = mlibc::sysdep_or_enosys<VmProtect>(
		    reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(map) + *guard_size), *stack_size, PROT_READ | PROT_WRITE
		);
		if(ret)
			return EAGAIN;
	}

	*stack_base = reinterpret_cast<void*>(map);
	auto sp = reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(map) + *guard_size + *stack_size);
	*--sp = reinterpret_cast<uintptr_t>(user_arg);
	*--sp = reinterpret_cast<uintptr_t>(entry);
	*stack = reinterpret_cast<void*>(sp);
	return 0;
}

} // namespace mlibc
