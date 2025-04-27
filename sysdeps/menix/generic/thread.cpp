#include <errno.h>
#include <menix/syscall.hpp>
#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/arch-defs.hpp>
#include <mlibc/tcb.hpp>
#include <sys/mman.h>

extern "C" void __mlibc_enter_thread(void *entry, void *user_arg, Tcb *tcb) {
	while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);
	}

	tcb->invokeThreadFunc(entry, user_arg);

	__atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&tcb->didExit);

	mlibc::sys_thread_exit();
}

// Defined in <arch>/thread.S
extern "C" void __mlibc_start_thread();

#define DEFAULT_STACK 0x200000

namespace mlibc {

int sys_clone(void *tcb, pid_t *pid_out, void *stack) {
	(void)tcb;
	auto r = menix_syscall(SYSCALL_THREAD_CREATE, (size_t)__mlibc_start_thread, (size_t)stack);
	if (r.error)
		return r.error;
	*pid_out = (pid_t)r.value;
	return 0;
}

int sys_prepare_stack(
    void **stack,
    void *entry,
    void *arg,
    void *tcb,
    size_t *stack_size,
    size_t *guard_size,
    void **stack_base
) {
	*guard_size = mlibc::page_size;

	*stack_size = *stack_size ? *stack_size : DEFAULT_STACK;

	if (!*stack) {
		*stack_base = mmap(
		    NULL,
		    *stack_size + mlibc::page_size,
		    PROT_READ | PROT_WRITE,
		    MAP_ANONYMOUS | MAP_PRIVATE,
		    -1,
		    0
		);
		if (*stack_base == MAP_FAILED) {
			return errno;
		}
		munmap((char *)*stack_base + *stack_size, mlibc::page_size);
	} else {
		*stack_base = *stack;
	}

	*stack = (void *)((char *)*stack_base + *stack_size);

	void **stack_it = (void **)*stack;

	*--stack_it = tcb;   // a2
	*--stack_it = arg;   // a1
	*--stack_it = entry; // a0

	*stack = (void *)stack_it;

	return 0;
}

} // namespace mlibc
