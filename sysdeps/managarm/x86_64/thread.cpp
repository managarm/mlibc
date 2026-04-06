#include <bits/ensure.h>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/thread-entry.hpp>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg, Tcb *tcb) {
	if (mlibc::sysdep<TcbSet>(tcb))
		__ensure(!"sys_tcb_set() failed");

	// Wait until our parent sets up the TID.
	while (!__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED))
		mlibc::sysdep<FutexWait>(&tcb->tid, 0, nullptr);

	// Enable cancellation once the TCB is up
	__atomic_fetch_or(&tcb->cancelBits, tcbCancelEnableBit, __ATOMIC_RELAXED);

	tcb->invokeThreadFunc(entry, user_arg);

	mlibc::thread_exit(tcb->returnValue);
}

namespace mlibc {

static constexpr size_t default_stacksize = 0x200000;

int Sysdeps<PrepareStack>::operator()(
    void **stack,
    void *entry,
    void *user_arg,
    void *tcb,
    size_t *stack_size,
    size_t *guard_size,
    void **stack_base
) {
	if (!*stack_size)
		*stack_size = default_stacksize;
	*guard_size = 0;

	if (*stack) {
		*stack_base = *stack;
	} else {
		*stack_base =
		    mmap(nullptr, *stack_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (*stack_base == MAP_FAILED) {
			return errno;
		}
	}

	uintptr_t *sp =
	    reinterpret_cast<uintptr_t *>(reinterpret_cast<uintptr_t>(*stack_base) + *stack_size);

	*--sp = reinterpret_cast<uintptr_t>(tcb);
	*--sp = reinterpret_cast<uintptr_t>(user_arg);
	*--sp = reinterpret_cast<uintptr_t>(entry);
	*stack = reinterpret_cast<void *>(sp);
	return 0;
}

} // namespace mlibc
