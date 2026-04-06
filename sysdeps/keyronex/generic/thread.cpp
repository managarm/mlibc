#include <keyronex/syscall.h>

#include <sys/mman.h>

#include <errno.h>

#include <mlibc/debug.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <bits/ensure.h>
#include <mlibc/tcb.hpp>

#define DEFAULT_STACK 0x400000

namespace mlibc {

extern "C" void __mlibc_thread_entry();

int
Sysdeps<Clone>::operator()(void *tcb, pid_t *tid_out, void *stack)
{
	(void)tcb;

	auto r = syscall2(SYS_thread_create, (uintptr_t)__mlibc_thread_entry, (uintptr_t)stack, NULL);
	if (r > 0) {
		*tid_out = r;
		return 0;
	}
	__ensure(r != 0);

	return r;
}

int
Sysdeps<PrepareStack>::operator()(void **stack, void *entry, void *arg, void *tcb,
    size_t *stack_size, size_t *guard_size, void **stack_base)
{
	// TODO guard

	mlibc::infoLogger() << "mlibc: sys_prepare_stack() does not setup a guard!" << frg::endlog;

	*guard_size = 0;

	*stack_size = *stack_size ? *stack_size : DEFAULT_STACK;

	if (!*stack) {
		*stack_base = mmap(NULL, *stack_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		if (*stack_base == MAP_FAILED) {
			return errno;
		}
	} else {
		*stack_base = *stack;
	}

	*stack = (void *)((char *)*stack_base + *stack_size);

	void **stack_it = (void **)*stack;

	*--stack_it = NULL;
	*--stack_it = arg;
	*--stack_it = tcb;
	*--stack_it = entry;

	*stack = (void *)stack_it;

	return 0;
}

void
Sysdeps<ThreadExit>::operator()()
{
	syscall0(SYS_thread_exit, NULL);
	__builtin_unreachable();
}

pid_t
Sysdeps<GetTid>::operator()()
{
	return syscall0(SYS_thread_gettid, NULL);
}

} /* namespace mlibc */

extern "C" void __mlibc_thread_trampoline(void *(*fn)(void *), Tcb *tcb, void *arg) {
	if (mlibc::sysdep<TcbSet>(tcb)) {
		__ensure(!"failed to set tcb for new thread");
	}

	while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
		mlibc::sysdep<FutexWait>(&tcb->tid, 0, nullptr);
	}

	// Enable cancellation once the TCB is up
	__atomic_fetch_or(&tcb->cancelBits, tcbCancelEnableBit, __ATOMIC_RELAXED);

	tcb->invokeThreadFunc(reinterpret_cast<void *>(fn), arg);

	mlibc::thread_exit(tcb->returnValue);
}
