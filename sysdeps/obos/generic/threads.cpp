#include <mlibc/ansi-sysdeps.hpp>
#include <mlibc/internal-sysdeps.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/tcb.hpp>

#include <obos/error.h>
#include <obos/syscall.h>
#include <obos/vma.h>

#include <errno.h>

extern "C" void __mlibc_thread_entry();

extern "C" void __mlibc_thread_trampoline(void (*fn)(uintptr_t user), Tcb *tcb, uintptr_t user) {
	mlibc::sys_tcb_set(tcb);

	// wait on the TID to be set
	while (__atomic_load_n(&tcb->tid, __ATOMIC_RELAXED) == 0)
		mlibc::sys_futex_wait(&tcb->tid, 0, nullptr);

	// invoke the entry
	tcb->invokeThreadFunc(reinterpret_cast<void *>(fn), (void *)user);

	__atomic_store_n(&tcb->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&tcb->didExit, true);

	syscall0(Sys_ExitCurrentThread);
}

int mlibc::sys_clone(void *tcb, pid_t *pid_out, void *stack) {
	handle ctx =
	    syscall5(Sys_ThreadContextCreate, __mlibc_thread_entry, 0, stack, 0, HANDLE_CURRENT);
	if (ctx == HANDLE_INVALID)
		return EINVAL;

#ifdef __SIZEOF_INT128__
	handle thr = syscall4(Sys_ThreadCreate, 2 /*THREAD_PRIORITY_NORMAL*/, 0, 0, ctx);
#else
	handle thr = syscall3(Sys_ThreadCreate, 2, 0, ctx);
#endif
	if (thr == HANDLE_INVALID) {
		syscall1(Sys_HandleClose, ctx);
		return EINVAL;
	}
	syscall2(Sys_ThreadSetOwner, thr, HANDLE_CURRENT);
	*pid_out = syscall1(Sys_ThreadGetTid, thr);

	syscall1(Sys_ThreadReady, thr);

	syscall1(Sys_HandleClose, ctx);
	syscall1(Sys_HandleClose, thr);

	return 0;
}
