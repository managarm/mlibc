#include <abi-bits/errno.h>
#include <bits/threads.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/threads.hpp>
#include <mlibc/tcb.hpp>

extern "C" Tcb *__rtdl_allocateTcb();

namespace mlibc {

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int) {
	auto new_tcb = __rtdl_allocateTcb();
	pid_t tid;
	struct __mlibc_threadattr attr = {};
	if (!attrp)
		thread_attr_init(&attr);
	else
		attr = *attrp;

	if (attr.__mlibc_cpuset)
		mlibc::infoLogger() << "pthread_create(): cpuset is ignored!" << frg::endlog;
	if (attr.__mlibc_sigmaskset)
		mlibc::infoLogger() << "pthread_create(): sigmask is ignored!" << frg::endlog;

	// TODO: due to alignment guarantees, the stackaddr and stacksize might change
	// when the stack is allocated. Currently this isn't propagated to the TCB,
	// but it should be.
	void *stack = attr.__mlibc_stackaddr;
	if (!mlibc::sys_prepare_stack) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	int ret = mlibc::sys_prepare_stack(&stack, entry,
			user_arg, new_tcb, &attr.__mlibc_stacksize, &attr.__mlibc_guardsize);
	if (ret)
		return ret;

	if (!mlibc::sys_clone) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	new_tcb->stackSize = attr.__mlibc_stacksize;
	new_tcb->guardSize = attr.__mlibc_guardsize;
	new_tcb->stackAddr = reinterpret_cast<void*>(
			reinterpret_cast<uintptr_t>(stack)
			- attr.__mlibc_stacksize - attr.__mlibc_guardsize);
	new_tcb->returnValueType = (returns_int) ? TcbThreadReturnValue::Integer : TcbThreadReturnValue::Pointer;
	mlibc::sys_clone(new_tcb, &tid, stack);
	*thread = reinterpret_cast<struct __mlibc_thread_data *>(new_tcb);

	__atomic_store_n(&new_tcb->tid, tid, __ATOMIC_RELAXED);
	mlibc::sys_futex_wake(&new_tcb->tid);

	return 0;
}

static constexpr size_t default_stacksize = 0x200000;
static constexpr size_t default_guardsize = 4096;

int thread_attr_init(struct __mlibc_threadattr *attr) {
	*attr = __mlibc_threadattr{};
	attr->__mlibc_stacksize = default_stacksize;
	attr->__mlibc_guardsize = default_guardsize;
	attr->__mlibc_detachstate = __MLIBC_THREAD_CREATE_JOINABLE;
	return 0;
}

} // namespace mlibc
