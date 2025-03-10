#include <sys/mman.h>
#include <mlibc/debug.hpp>
#include <errno.h>
#include <mlibc/all-sysdeps.hpp>
#include <bits/ensure.h>
#include <mlibc/tcb.hpp>
#include <mlibc/arch-defs.hpp>

extern "C" void __mlibc_thread_trampoline(void *(*fn)(void *), Tcb *tcb, void *arg) {
	while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0) {
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);
	}

	tcb->invokeThreadFunc(reinterpret_cast<void *>(fn), arg);

	__atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&tcb->didExit);

	mlibc::sys_thread_exit();
}

#define DEFAULT_STACK 0x20000

namespace mlibc {
	int sys_prepare_stack(void **stack, void *entry, void *arg, void *tcb, size_t *stack_size, size_t *guard_size, void **stack_base) {
		*guard_size = mlibc::page_size;

		*stack_size = *stack_size ? *stack_size : DEFAULT_STACK;

		if (!*stack) {
			*stack_base = mmap(NULL, *stack_size + mlibc::page_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
			if (*stack_base == MAP_FAILED) {
				return errno;
			}
			munmap((char *)*stack_base + *stack_size, mlibc::page_size);
		} else {
			*stack_base = *stack;
		}

		*stack = (void *)((char *)*stack_base + *stack_size);

		void **stack_it = (void **)*stack;

		*--stack_it = arg;
		*--stack_it = tcb;
		*--stack_it = entry;

		*stack = (void *)stack_it;

		return 0;
	}
}
