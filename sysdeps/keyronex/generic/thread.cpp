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
sys_clone(void *tcb, pid_t *tid_out, void *stack)
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
sys_prepare_stack(void **stack, void *entry, void *arg, void *tcb,
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
sys_thread_exit()
{
	syscall0(SYS_thread_exit, NULL);
	__builtin_unreachable();
}

pid_t
sys_gettid()
{
	return syscall0(SYS_thread_gettid, NULL);
}

} /* namespace mlibc */

extern "C" void __mlibc_thread_trampoline(void *(*fn)(void *), Tcb *tcb, void *arg) {
	if (mlibc::sys_tcb_set(tcb)) {
		__ensure(!"failed to set tcb for new thread");
	}

	while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);
	}

	tcb->invokeThreadFunc(reinterpret_cast<void *>(fn), arg);

	__atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&tcb->didExit);

	mlibc::sys_thread_exit();
}

#if defined (__m68k__)
extern "C" void *
__m68k_read_tp (void)
{
	return mlibc::sys_tp_get();
}
#endif
