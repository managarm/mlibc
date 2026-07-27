
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#if __MLIBC_BSD_OPTION
#include <pthread_np.h>
#endif

#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <frg/array.hpp>
#include <frg/bitops.hpp>
#include <frg/scope_exit.hpp>
#include <frg/spinlock.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/tid.hpp>
#include <mlibc/threads.hpp>
#include <mlibc/thread-types.hpp>
#include <mlibc/time-helpers.hpp>
#include <mlibc/global-config.hpp>

struct ScopeTrace {
	ScopeTrace(const char *file, int line, const char *function)
	: _file(file), _line(line), _function(function) {
		if(!mlibc::globalConfig().debugPthreadTrace)
			return;
		mlibc::infoLogger() << "trace: Enter scope "
				<< _file << ":" << _line << " (in function "
				<< _function << ")" << frg::endlog;
	}

	~ScopeTrace() {
		if(!mlibc::globalConfig().debugPthreadTrace)
			return;
		mlibc::infoLogger() << "trace: Exit scope" << frg::endlog;
	}

private:
	const char *_file;
	int _line;
	const char *_function;
};

#define SCOPE_TRACE() ScopeTrace(__FILE__, __LINE__, __FUNCTION__)

static_assert(sizeof(__mlibc_mutex) == sizeof(pthread_mutex_t));
static_assert(alignof(__mlibc_mutex) == sizeof(uintptr_t));
static_assert(sizeof(__mlibc_mutex) == __MLIBC_THREAD_MUTEX_SIZE);
static_assert(sizeof(__mlibc_cond) == sizeof(pthread_cond_t));
static_assert(alignof(__mlibc_cond) == sizeof(uintptr_t));
static_assert(sizeof(__mlibc_cond) == __MLIBC_THREAD_COND_SIZE);
static_assert(sizeof(__mlibc_barrier) == sizeof(pthread_barrier_t));
static_assert(alignof(__mlibc_barrier) == sizeof(uintptr_t));
static_assert(sizeof(__mlibc_barrier) == __MLIBC_THREAD_BARRIER_SIZE);
static_assert(sizeof(__mlibc_fair_rwlock) == sizeof(pthread_rwlock_t));
static_assert(alignof(__mlibc_fair_rwlock) == sizeof(uintptr_t));
static_assert(sizeof(__mlibc_fair_rwlock) == __MLIBC_THREAD_RWLOCK_SIZE);
static_assert(sizeof(__mlibc_once) == sizeof(pthread_once_t));
static_assert(sizeof(__mlibc_once) == __MLIBC_THREAD_ONCE_SIZE);
static_assert(sizeof(__mlibc_spinlock) == sizeof(pthread_spinlock_t));
static_assert(sizeof(__mlibc_spinlock) == __MLIBC_THREAD_SPINLOCK_SIZE);

// ----------------------------------------------------------------------------
// pthread_attr and pthread functions.
// ----------------------------------------------------------------------------

// pthread_attr functions.
int pthread_attr_init(pthread_attr_t *attr) {
	attr->__heap_ptr = frg::construct<__mlibc_threadattr>(getAllocator());
	return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_threadattr>(getAllocator(), __mlibc_threadattr::from(attr));
		attr->__heap_ptr = nullptr;
	}

	return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	auto a = __mlibc_threadattr::from(attr);
	*detachstate = a->__detachstate ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE;
	return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	if (detachstate != PTHREAD_CREATE_DETACHED &&
			detachstate != PTHREAD_CREATE_JOINABLE)
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	a->__detachstate = detachstate == PTHREAD_CREATE_DETACHED;
	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *__restrict attr, size_t *__restrict stacksize) {
	auto a = __mlibc_threadattr::from(attr);
	*stacksize = a->__stacksize;
	return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	if (stacksize < PTHREAD_STACK_MIN)
		return EINVAL;
	auto a = __mlibc_threadattr::from(attr);
	a->__stacksize = stacksize;
	return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
	auto a = __mlibc_threadattr::from(attr);
	*stackaddr = a->__stackaddr;
	return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	auto a = __mlibc_threadattr::from(attr);
	a->__stackaddr = stackaddr;
	return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
	auto a = __mlibc_threadattr::from(attr);
	*stackaddr = a->__stackaddr;
	*stacksize = a->__stacksize;
	return 0;
}

int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
	if (stacksize < PTHREAD_STACK_MIN)
		return EINVAL;
	auto a = __mlibc_threadattr::from(attr);
	a->__stacksize = stacksize;
	a->__stackaddr = stackaddr;
	return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t *__restrict attr, size_t *__restrict guardsize) {
	auto a = __mlibc_threadattr::from(attr);
	*guardsize = a->__guardsize;
	return 0;
}

int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
	auto a = __mlibc_threadattr::from(attr);
	a->__guardsize = guardsize;
	return 0;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope) {
	auto a = __mlibc_threadattr::from(attr);
	*scope = a->__scope ? PTHREAD_SCOPE_PROCESS : PTHREAD_SCOPE_SYSTEM;
	return 0;
}

int pthread_attr_setscope(pthread_attr_t *attr, int scope) {
	if (scope != PTHREAD_SCOPE_SYSTEM && scope != PTHREAD_SCOPE_PROCESS)
		return EINVAL;
	if (scope == PTHREAD_SCOPE_PROCESS)
		return ENOTSUP;
	auto a = __mlibc_threadattr::from(attr);
	a->__scope = (scope == PTHREAD_SCOPE_PROCESS);
	return 0;
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
	auto a = __mlibc_threadattr::from(attr);
	*inheritsched = a->__inheritsched ? PTHREAD_EXPLICIT_SCHED : PTHREAD_INHERIT_SCHED;
	return 0;
}

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
	if (inheritsched != PTHREAD_INHERIT_SCHED &&
			inheritsched != PTHREAD_EXPLICIT_SCHED) [[unlikely]]
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	a->__inheritsched = (inheritsched == PTHREAD_EXPLICIT_SCHED);
	return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *__restrict attr, struct sched_param *__restrict schedparam) {
	auto a = __mlibc_threadattr::from(attr);
	memcpy(schedparam, &a->__schedparam, sizeof(*schedparam));
	return 0;
}

int pthread_attr_setschedparam(pthread_attr_t *__restrict attr, const struct sched_param *__restrict schedparam) {
	// TODO: this is supposed to return EINVAL for when the schedparam doesn't make sense
	// for the given schedpolicy.
	auto a = __mlibc_threadattr::from(attr);
	memcpy(&a->__schedparam, schedparam, sizeof(a->__schedparam));
	return 0;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict attr, int *__restrict policy) {
	auto a = __mlibc_threadattr::from(attr);
	*policy = a->__schedpolicy;
	return 0;
}

int pthread_attr_setschedpolicy(pthread_attr_t *__restrict attr, int policy) {
	if (policy != SCHED_FIFO && policy != SCHED_RR &&
			policy != SCHED_OTHER)
		return EINVAL;
	auto a = __mlibc_threadattr::from(attr);
	a->__schedpolicy = policy;
	return 0;
}

namespace {
	int get_own_stackinfo(void **stack_addr, size_t *stack_size) {
		if constexpr (mlibc::IsImplemented<GetCurrentStackInfo>) {
			return mlibc::sysdep_or_enosys<GetCurrentStackInfo>(stack_addr, stack_size);
		}

#if __MLIBC_LINUX_OPTION
		// Fallback to /proc/self/maps
		auto fp = fopen("/proc/self/maps", "r");
		if (!fp) {
			mlibc::infoLogger() << "mlibc pthreads: /proc/self/maps does not exist! Returning ENOSYS." << frg::endlog;
			return ENOSYS;
		}

		char line[256];
		auto sp = mlibc::get_sp();
		while (fgets(line, 256, fp)) {
			uintptr_t from, to;
			if(sscanf(line, "%" SCNxPTR "-%" SCNxPTR, &from, &to) != 2)
				continue;
			if (sp < to && sp > from) {
				// We need to return the lowest byte of the stack.
				*stack_addr = reinterpret_cast<void*>(from);
				*stack_size = to - from;
				fclose(fp);
				return 0;
			}
		}

		fclose(fp);
		return ESRCH;
#else
		return ENOSYS;
#endif // __MLIBC_LINUX_OPTION
	}
} // namespace

#if __MLIBC_LINUX_OPTION
int pthread_attr_getaffinity_np(const pthread_attr_t *__restrict attr,
		size_t cpusetsize, cpu_set_t *__restrict cpusetp) {
	if (!attr)
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	if (!a->__cpuset) {
		memset(cpusetp, -1, cpusetsize);
		return 0;
	}

	for (size_t cnt = cpusetsize; cnt < a->__cpusetsize; cnt++)
		if (reinterpret_cast<char*>(a->__cpuset)[cnt] != '\0')
			return ERANGE;

	auto p = memcpy(cpusetp, a->__cpuset,
			std::min(cpusetsize, a->__cpusetsize));
	if (cpusetsize > a->__cpusetsize)
		memset(p, '\0', cpusetsize - a->__cpusetsize);

	return 0;
}

int pthread_attr_setaffinity_np(pthread_attr_t *__restrict attr,
		size_t cpusetsize, const cpu_set_t *__restrict cpusetp) {
	if (!attr)
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	if (!cpusetp || !cpusetsize) {
		a->__cpuset = nullptr;
		a->__cpusetsize = 0;
		return 0;
	}

	if (a->__cpusetsize != cpusetsize) {
		auto newp = realloc(a->__cpuset, cpusetsize);
		if (!newp)
			return ENOMEM;

		a->__cpuset = static_cast<cpu_set_t *>(newp);
		a->__cpusetsize = cpusetsize;
	}

	memcpy(a->__cpuset, cpusetp, cpusetsize);
	return 0;
}

int pthread_attr_getsigmask_np(const pthread_attr_t *__restrict attr,
		sigset_t *__restrict sigmask) {
	if (!attr)
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	if (!a->__sigmaskset) {
		sigemptyset(sigmask);
		return PTHREAD_ATTR_NO_SIGMASK_NP;
	}

	*sigmask = a->__sigmask;

	return 0;
}
int pthread_attr_setsigmask_np(pthread_attr_t *__restrict attr,
		const sigset_t *__restrict sigmask) {
	if (!attr)
		return EINVAL;

	auto a = __mlibc_threadattr::from(attr);
	if (!sigmask) {
		a->__sigmaskset = 0;
		return 0;
	}

	a->__sigmask = *sigmask;
	a->__sigmaskset = 1;

	// Filter out internally used signals.
	sigdelset(&a->__sigmask, SIGCANCEL);

	return 0;
}

int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	// The Linux version explicitly initializes the attr, unlike the BSD version.
	pthread_attr_init(attr);
	auto a = __mlibc_threadattr::from(attr);

	if (!tcb->stackAddr || !tcb->stackSize) {
		if (int err = get_own_stackinfo(&a->__stackaddr, &a->__stacksize); err) {
			return err;
		}
	} else {
		a->__stacksize = tcb->stackSize;
		a->__stackaddr = tcb->stackAddr;
	}

	a->__guardsize = tcb->guardSize;
	a->__detachstate = tcb->isJoinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;
	mlibc::infoLogger() << "pthread_getattr_np(): Implementation is incomplete!" << frg::endlog;
	return 0;
}

int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *mask) {
	return mlibc::sysdep_or_enosys<GetThreadaffinity>(reinterpret_cast<Tcb*>(thread)->tid, cpusetsize, mask);
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *mask) {
	return mlibc::sysdep_or_enosys<SetThreadaffinity>(reinterpret_cast<Tcb*>(thread)->tid, cpusetsize, mask);
}
#endif // __MLIBC_LINUX_OPTION

#if __MLIBC_BSD_OPTION
int pthread_attr_get_np(pthread_t thread, pthread_attr_t *attr) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	// The Linux version explicitly initializes the attr, unlike the BSD version.
	auto a = __mlibc_threadattr::from(attr);

	if (!tcb->stackAddr || !tcb->stackSize) {
		if (int err = get_own_stackinfo(&a->__stackaddr, &a->__stacksize); err) {
			return err;
		}
	} else {
		a->__stacksize = tcb->stackSize;
		a->__stackaddr = tcb->stackAddr;
	}

	a->__guardsize = tcb->guardSize;
	a->__detachstate = tcb->isJoinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;
	return 0;
}
#endif // __MLIBC_BSD_OPTION

extern "C" Tcb *__rtld_allocateTcb();

// pthread functions.
int pthread_create(pthread_t *__restrict thread, const pthread_attr_t *__restrict attrp,
		void *(*entry) (void *), void *__restrict user_arg) {
	auto attr = attrp ? __mlibc_threadattr::from(attrp) : nullptr;
	return mlibc::thread_create(thread, attr, reinterpret_cast<void *>(entry), user_arg, false);
}

pthread_t pthread_self(void) {
	return reinterpret_cast<pthread_t>(mlibc::get_current_tcb());
}

int pthread_equal(pthread_t t1, pthread_t t2) {
	if(t1 == t2)
		return 1;
	return 0;
}

__attribute__ ((__noreturn__)) void pthread_exit(void *ret_val) {
	// POSIX: when a thread calls pthread_exit(), the thread first disables cancellation by setting
	// its cancelability to PTHREAD_CANCEL_DISABLE + PTHREAD_CANCEL_DEFERRED; this should remain in
	// place until the thread has terminated.
	auto self = mlibc::get_current_tcb();
	int old_value = __atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED);
	int new_value;

	while (1) {
		new_value = old_value & ~(tcbCancelAsyncBit | tcbCancelEnableBit);

		// Avoid unnecessary atomic op.
		if (old_value == new_value)
			break;

		if (__atomic_compare_exchange_n(
		        &self->cancelBits,
		        &old_value,
		        new_value,
		        true,
		        __ATOMIC_RELAXED,
		        __ATOMIC_RELAXED
		    ))
			break;
	}

	mlibc::thread_exit({.voidPtr = ret_val});
}

int pthread_join(pthread_t thread, void **ret) {
	return mlibc::thread_join(thread, ret);
}

int pthread_detach(pthread_t thread) {
	return mlibc::thread_detach(thread);
}

void pthread_cleanup_push(void (*func) (void *), void *arg) {
	auto self = mlibc::get_current_tcb();

	auto hand = frg::construct<Tcb::CleanupHandler>(getAllocator());
	hand->func = func;
	hand->arg = arg;
	self->cleanupHandlers.push_back(hand);
}

void pthread_cleanup_pop(int execute) {
	auto self = mlibc::get_current_tcb();

	auto hand = self->cleanupHandlers.pop_back();

	if (execute)
		hand->func(hand->arg);

	frg::destruct(getAllocator(), hand);
}

int pthread_setname_np(pthread_t thread, const char *name) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	return mlibc::sysdep_or_enosys<ThreadSetname>(tcb, name);
}

int pthread_getname_np(pthread_t thread, char *name, size_t size) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	return mlibc::sysdep_or_enosys<ThreadGetname>(tcb, name, size);
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	return mlibc::sysdep_or_enosys<SetSchedparam>(tcb, policy, param);
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	return mlibc::sysdep_or_enosys<GetSchedparam>(tcb, policy, param);
}

//pthread cancel functions

namespace {

	void sigcancel_handler(int signal, siginfo_t *info, void *ucontext) {
		ucontext_t *uctx = static_cast<ucontext_t*>(ucontext);
		// The function could be called from other signals, or from another
		// process, in which case we should do nothing.
		if (signal != SIGCANCEL || info->si_pid != getpid() ||
				info->si_code != SI_TKILL)
			return;

		auto tcb = mlibc::get_current_tcb();
		int old_value = __atomic_load_n(&tcb->cancelBits, __ATOMIC_RELAXED);

		/*
		 * When a thread is marked with deferred cancellation and performs a blocking syscall,
		 * the spec mandates that the syscall can get interrupted before it has caused any side
		 * effects (e.g. before a read() has read any bytes from disk). If the syscall has
		 * already caused side effects it should return its partial work, and set the program
		 * counter just after the syscall. If the syscall hasn't caused any side effects, it
		 * should fail with EINTR and set the program counter to the syscall instruction.
		 *
		 *	cancellable_syscall:
		 *		test whether_a_cancel_is_queued
		 *		je cancel
		 *		syscall
		 *	end_cancellable_syscall
		 *
		 * The mlibc::sys_before_cancellable_syscall sysdep should return 1 when the
		 * program counter is between the 'canellable_syscall' and 'end_cancellable_syscall' label.
		 */
		if (!(old_value & tcbCancelAsyncBit) &&
				mlibc::IsImplemented<BeforeCancellableSyscall> && !mlibc::sysdep_or_enosys<BeforeCancellableSyscall>(uctx))
			return;

		int bitmask = tcbCancelTriggerBit | tcbCancelingBit;
		while (1) {
			int new_value = old_value | bitmask;

			// Check if we are already cancelled or exiting
			if (old_value == new_value || old_value & tcbExitingBit)
				return;

			int current_value = old_value;
			if (__atomic_compare_exchange_n(&tcb->cancelBits, &current_value,
						new_value, true,__ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
				tcb->returnValue.voidPtr = PTHREAD_CANCELED;

				// Perform cancellation
				__mlibc_do_cancel();

				break;
			}

			old_value = current_value;
		}
	}
} // namespace

namespace mlibc {
namespace {

struct PthreadSignalInstaller {
	PthreadSignalInstaller() {
		struct sigaction sa{};
		sa.sa_sigaction = sigcancel_handler;
		sa.sa_flags = SA_SIGINFO;
		auto e = ENOSYS;
		if constexpr (mlibc::IsImplemented<Sigaction>)
			e = mlibc::sysdep_or_panic<Sigaction>(SIGCANCEL, &sa, nullptr);
		// Opt-out of cancellation support.
		if(e == ENOSYS)
			return;
		__ensure(!e);
	}
};

PthreadSignalInstaller pthread_signal_installer;

} // anonymous namespace
} // namespace mlibc

int pthread_setcanceltype(int type, int *oldtype) {
	if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS)
		return EINVAL;

	auto self = mlibc::get_current_tcb();
	int old_value = __atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED);
	while (1) {
		int new_value = old_value & ~tcbCancelAsyncBit;
		if (type == PTHREAD_CANCEL_ASYNCHRONOUS)
			new_value |= tcbCancelAsyncBit;

		if (oldtype)
			*oldtype = ((old_value & tcbCancelAsyncBit)
					? PTHREAD_CANCEL_ASYNCHRONOUS
					: PTHREAD_CANCEL_DEFERRED);

		// Avoid unecessary atomic op.
		if (old_value == new_value)
			break;

		int current_value = old_value;
		if (__atomic_compare_exchange_n(&self->cancelBits, &current_value,
					new_value, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {

			if (mlibc::tcb_async_cancelled(new_value))
				__mlibc_do_cancel();

			break;
		}

		old_value = current_value;
	}

	return 0;
}

int pthread_setcancelstate(int state, int *oldstate) {
	if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE)
		return EINVAL;

	auto self = mlibc::get_current_tcb();
	int old_value = __atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED);
	while (1) {
		int new_value = old_value & ~tcbCancelEnableBit;
		if (state == PTHREAD_CANCEL_ENABLE)
			new_value |= tcbCancelEnableBit;

		if (oldstate)
			*oldstate = ((old_value & tcbCancelEnableBit)
					? PTHREAD_CANCEL_ENABLE
					: PTHREAD_CANCEL_DISABLE);

		// Avoid unecessary atomic op.
		if (old_value == new_value)
			break;

		int current_value = old_value;
		if (__atomic_compare_exchange_n(&self->cancelBits, &current_value,
					new_value, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {

			if (mlibc::tcb_async_cancelled(new_value))
				__mlibc_do_cancel();

			sigset_t set = {};
			sigaddset(&set, SIGCANCEL);
			if (mlibc::tcb_cancel_enabled(new_value))
				sigprocmask(SIG_UNBLOCK, &set, nullptr);
			else
				sigprocmask(SIG_BLOCK, &set, nullptr);
			break;
		}

		old_value = current_value;
	}

	return 0;
}

void pthread_testcancel(void) {
	mlibc::thread_testcancel();
}

int pthread_cancel(pthread_t thread) {
	if constexpr (!mlibc::IsImplemented<Tgkill>) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}

	auto tcb = reinterpret_cast<Tcb *>(thread);
	// Check if the TCB is valid, somewhat..
	if (tcb->selfPointer != tcb)
		return ESRCH;

	int old_value = __atomic_load_n(&tcb->cancelBits, __ATOMIC_RELAXED);
	while (1) {
		int bitmask = tcbCancelTriggerBit;

		int new_value = old_value | bitmask;
		if (old_value == new_value)
			break;

		int current_value = old_value;
		if (__atomic_compare_exchange_n(&tcb->cancelBits, &current_value,
					new_value, true, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
			if (mlibc::tcb_cancel_enabled(new_value)) {
				if (thread == pthread_self()) {
					// optimization: if cancelling itself, we can avoid sending a signal
					if (mlibc::tcb_async_cancel(new_value))
						pthread_exit(PTHREAD_CANCELED);
					return 0;
				}

				pid_t pid = getpid();

				int res = mlibc::sysdep_or_panic<Tgkill>(pid, tcb->tid, SIGCANCEL);

				current_value = __atomic_load_n(&tcb->cancelBits, __ATOMIC_RELAXED);

				// If we can't find the thread anymore, it's possible that it exited between
				// us setting the cancel trigger bit, and us sending the signal. Check the
				// cancelBits for tcbExitingBit to confirm that.
				// XXX(qookie): This will be an use-after-free once we start freeing TCBs on
				//              exit. Perhaps the TCB should be refcounted.
				if (!(res == ESRCH && (current_value & tcbExitingBit)))
					return res;
			}

			break;
		}

		old_value = current_value;
	}

	return 0;
}

int pthread_atfork(void (*prepare) (void), void (*parent) (void), void (*child) (void)) {
	auto self = mlibc::get_current_tcb();

	auto hand = frg::construct<Tcb::AtforkHandler>(getAllocator());
	if (!hand)
		return ENOMEM;

	hand->prepare = prepare;
	hand->parent = parent;
	hand->child = child;
	hand->next = nullptr;
	hand->prev = self->atforkEnd;

	if (self->atforkEnd)
		self->atforkEnd->next = hand;

	self->atforkEnd = hand;

	if (!self->atforkBegin)
		self->atforkBegin = self->atforkEnd;

	return 0;
}

// ----------------------------------------------------------------------------
// pthread_key functions.
// ----------------------------------------------------------------------------

int pthread_key_create(pthread_key_t *out, void (*destructor)(void *)) {
	SCOPE_TRACE();

	return mlibc::thread_key_create(out, destructor);
}

int pthread_key_delete(pthread_key_t key) {
	SCOPE_TRACE();

	mlibc::thread_key_delete(key);
	return 0;
}

void *pthread_getspecific(pthread_key_t key) {
	SCOPE_TRACE();

	return mlibc::thread_key_get(key);
}

int pthread_setspecific(pthread_key_t key, const void *value) {
	SCOPE_TRACE();

	return mlibc::thread_key_set(key, value);
}

// ----------------------------------------------------------------------------
// pthread_once functions.
// ----------------------------------------------------------------------------

int pthread_once(pthread_once_t *once, void (*function) (void)) {
	SCOPE_TRACE();
	return mlibc::thread_once(__mlibc_once::from(once), function);
}

// ----------------------------------------------------------------------------
// pthread_mutexattr and pthread_mutex functions.
// ----------------------------------------------------------------------------

// pthread_mutexattr functions
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	SCOPE_TRACE();
	attr->__heap_ptr = frg::construct<__mlibc_mutexattr>(getAllocator());
	return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	SCOPE_TRACE();
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_mutexattr>(getAllocator(), __mlibc_mutexattr::from(attr));
		attr->__heap_ptr = nullptr;
	}
	return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict attr, int *__restrict type) {
	return mlibc::thread_mutexattr_gettype(__mlibc_mutexattr::from(attr), type);
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	return mlibc::thread_mutexattr_settype(__mlibc_mutexattr::from(attr), type);
}

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict attr,
		int *__restrict robust) {
	// *robust = attr->__mlibc_robust;
	// return 0;
	(void) attr;
	(void) robust;
	mlibc::infoLogger() << "mlibc: POSIX_ROBUST_MUTEXES are unsupported!" << frg::endlog;
	return ENOSYS;
}
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust) {
	// if (robust != PTHREAD_MUTEX_STALLED && robust != PTHREAD_MUTEX_ROBUST)
	// 	return EINVAL;

	// attr->__mlibc_robust = robust;
	// return 0;
	(void) attr;
	(void) robust;
	mlibc::infoLogger() << "mlibc: POSIX_ROBUST_MUTEXES are unsupported!" << frg::endlog;
	return ENOSYS;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
	auto a = __mlibc_mutexattr::from(attr);
	*pshared = a->__mlibc_pshared;
	return 0;
}
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	auto a = __mlibc_mutexattr::from(attr);
	a->__mlibc_pshared = pshared;
	return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict attr,
		int *__restrict protocol) {
	auto a = __mlibc_mutexattr::from(attr);
	*protocol = a->__mlibc_protocol;
	return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
	if (protocol != PTHREAD_PRIO_NONE && protocol != PTHREAD_PRIO_INHERIT
			&& protocol != PTHREAD_PRIO_PROTECT)
		return EINVAL;

	auto a = __mlibc_mutexattr::from(attr);
	a->__mlibc_protocol = protocol;
	return 0;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *__restrict attr,
		int *__restrict prioceiling) {
	(void)attr;
	(void)prioceiling;
	return EINVAL;
}

int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling) {
	(void)attr;
	(void)prioceiling;
	return EINVAL;
}

// pthread_mutex functions
int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
		const pthread_mutexattr_t *__restrict attr) {
	SCOPE_TRACE();

	const __mlibc_mutexattr *ma = attr ? __mlibc_mutexattr::from(attr) : nullptr;
	return mlibc::thread_mutex_init(__mlibc_mutex::from(mutex), ma);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	return mlibc::thread_mutex_destroy(__mlibc_mutex::from(mutex));
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_lock(__mlibc_mutex::from(mutex));
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_trylock(__mlibc_mutex::from(mutex));
}

int pthread_mutex_timedlock(pthread_mutex_t *__restrict mutex,
		const struct timespec *__restrict abstime) {
	return pthread_mutex_clocklock(mutex, CLOCK_REALTIME, abstime);
}

int pthread_mutex_clocklock(pthread_mutex_t *__restrict mutex,
		clockid_t clockid, const struct timespec *__restrict abstime) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_timedlock(__mlibc_mutex::from(mutex), abstime, clockid);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_unlock(__mlibc_mutex::from(mutex));
}

int pthread_mutex_consistent(pthread_mutex_t *) {
	mlibc::infoLogger() << "mlibc: pthread_mutex_consistent is unsupported!" << frg::endlog;
	return ENOSYS;
}

// ----------------------------------------------------------------------------
// pthread_condattr and pthread_cond functions.
// ----------------------------------------------------------------------------

int pthread_condattr_init(pthread_condattr_t *attr) {
	attr->__heap_ptr = frg::construct<__mlibc_condattr>(getAllocator());
	return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr) {
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_condattr>(getAllocator(), __mlibc_condattr::from(attr));
		attr->__heap_ptr = nullptr;
	}
	return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *__restrict attr,
		clockid_t *__restrict clock) {
	auto a = __mlibc_condattr::from(attr);
	*clock = a->__mlibc_clock;
	return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock) {
	if (clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC
			&& clock != CLOCK_MONOTONIC_RAW && clock != CLOCK_REALTIME_COARSE
			&& clock != CLOCK_MONOTONIC_COARSE && clock != CLOCK_BOOTTIME)
		return EINVAL;

	auto a = __mlibc_condattr::from(attr);
	a->__mlibc_clock = clock;
	return 0;
}

int pthread_condattr_getpshared(const pthread_condattr_t *__restrict attr,
		int *__restrict pshared) {
	auto a = __mlibc_condattr::from(attr);
	*pshared = a->__mlibc_pshared;
	return 0;
}

int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	auto a = __mlibc_condattr::from(attr);
	a->__mlibc_pshared = pshared;
	return 0;
}

int pthread_cond_init(pthread_cond_t *__restrict cond, const pthread_condattr_t *__restrict attr) {
	SCOPE_TRACE();

	return mlibc::thread_cond_init(__mlibc_cond::from(cond), __mlibc_condattr::from(attr));
}

int pthread_cond_destroy(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return mlibc::thread_cond_destroy(__mlibc_cond::from(cond));
}

int pthread_cond_wait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex) {
	return pthread_cond_timedwait(cond, mutex, nullptr);
}

int pthread_cond_timedwait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex,
		const struct timespec *__restrict abstime) {
	auto c = __mlibc_cond::from(cond);
	return mlibc::thread_cond_timedwait(c, __mlibc_mutex::from(mutex), abstime, c->__mlibc_clock);
}

int pthread_cond_clockwait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex,
		clockid_t clockid, const struct timespec *__restrict abstime) {
	return mlibc::thread_cond_timedwait(__mlibc_cond::from(cond), __mlibc_mutex::from(mutex), abstime, clockid);
}

int pthread_cond_signal(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return mlibc::thread_cond_signal(__mlibc_cond::from(cond));
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return mlibc::thread_cond_broadcast(__mlibc_cond::from(cond));
}

// ----------------------------------------------------------------------------
// pthread_barrierattr and pthread_barrier functions.
// ----------------------------------------------------------------------------

int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
	attr->__heap_ptr = frg::construct<__mlibc_barrierattr_struct>(getAllocator());
	return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict attr,
		int *__restrict pshared) {
	auto a = __mlibc_barrierattr_struct::from(attr);
	*pshared = a->__mlibc_pshared;
	return 0;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)
		return EINVAL;

	auto a = __mlibc_barrierattr_struct::from(attr);
	a->__mlibc_pshared = pshared;
	return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *attr) {
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_barrierattr_struct>(
		    getAllocator(), __mlibc_barrierattr_struct::from(attr)
		);
		attr->__heap_ptr = nullptr;
	}
	return 0;
}

int pthread_barrier_init(pthread_barrier_t *__restrict barrier,
		const pthread_barrierattr_t *__restrict attr, unsigned count) {
	if (count == 0)
		return EINVAL;

	auto b = __mlibc_barrier::from(barrier);
	const __mlibc_barrierattr_struct *a = attr ? __mlibc_barrierattr_struct::from(attr) : nullptr;
	new (b) __mlibc_barrier(count, a && a->__mlibc_pshared == PTHREAD_PROCESS_SHARED);

	return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
	auto b = __mlibc_barrier::from(barrier);

	while (true) {
		unsigned expected = b->__mlibc_inside.load(std::memory_order_acquire);
		if (expected == 0)
			break;

		int e = mlibc::sysdep<FutexWait>((int *)&b->__mlibc_inside, expected, nullptr);
		if (e != 0 && e != EAGAIN && e != EINTR)
			mlibc::panicLogger() << "mlibc: sys_futex_wait() returned error " << e << frg::endlog;
	}

	return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier) {
	auto b = __mlibc_barrier::from(barrier);

	// inside is incremented on entry and decremented on exit.
	// This is used to synchronise with pthread_barrier_destroy, to ensure that a thread doesn't
	// pass the barrier and immediately destroy its state while other threads still rely on it.
	b->__mlibc_inside.fetch_add(1, std::memory_order_acquire);

	auto leave = [&]() {
		unsigned inside = b->__mlibc_inside.fetch_sub(1, std::memory_order_release);
		if (inside == 0)
			mlibc::sysdep<FutexWake>((int *)&b->__mlibc_inside, true);
	};

	unsigned int count = b->__mlibc_count;
	unsigned int gen_shift = frg::ceil_log2(count);
	unsigned int mask = (gen_shift < 32) ? ((1U << gen_shift) - 1) : ~0U;

	unsigned int old = b->__mlibc_waiting.load(std::memory_order_relaxed);

	while (true) {
		unsigned int gen = (gen_shift < 32) ? (old >> gen_shift) : 0;
		unsigned int waiters = old & mask;

		if (waiters + 1 == count) {
			// Last thread of the generation.
			unsigned int next_gen_start = (gen + 1) << gen_shift;
			if (b->__mlibc_waiting.compare_exchange_weak(
			        old, next_gen_start, std::memory_order_acq_rel, std::memory_order_relaxed
			    )) {
				// Update the sequence number to match the new generation.
				// This acts as the signal for all threads in the current generation to leave.
				b->__mlibc_seq.store(gen + 1, std::memory_order_release);
				mlibc::sysdep<FutexWake>((int *)&b->__mlibc_seq, true);
				leave();
				return PTHREAD_BARRIER_SERIAL_THREAD;
			}
		} else {
			if (b->__mlibc_waiting.compare_exchange_weak(
			        old, old + 1, std::memory_order_acq_rel, std::memory_order_relaxed
			    )) {
				while (true) {
					unsigned int current_seq =
						b->__mlibc_seq.load(std::memory_order_acquire);
					if ((int)(current_seq - gen) > 0)
						break;
					int e = mlibc::sysdep<FutexWait>(
					    (int *)&b->__mlibc_seq, current_seq, nullptr
					);
					if (e != 0 && e != EAGAIN && e != EINTR)
						mlibc::panicLogger()
						    << "mlibc: sys_futex_wait() returned error " << e << frg::endlog;
				}
				leave();
				return 0;
			}
		}
	}
}

// ----------------------------------------------------------------------------
// pthread_rwlock functions.
// ----------------------------------------------------------------------------

namespace {
	int rwlock_m_lock(pthread_rwlock_t *rw, bool excl, clockid_t clock = CLOCK_MONOTONIC,
			const struct timespec *__restrict abstime = nullptr) {
		auto l = __mlibc_fair_rwlock::from(rw);
		unsigned int m_expected = l->__mlibc_m.load(std::memory_order_relaxed);
		while(true) {
			if(m_expected) {
				__ensure(m_expected & __mlibc_fair_rwlock::mutex_owner_mask);

				// Try to set the waiters bit.
				if(!(m_expected & __mlibc_fair_rwlock::mutex_waiters_bit)) {
					unsigned int desired = m_expected | __mlibc_fair_rwlock::mutex_waiters_bit;
				    if (!l->__mlibc_m.compare_exchange_strong(
				            m_expected,
				            desired,
				            std::memory_order_relaxed,
				            std::memory_order_relaxed
				        ))
					    continue;
				}

				// Wait on the futex.
				// Recompute the relative timeout on each iteration (in case of EAGAIN/EINTR wakeups).
				int e;
				if (abstime) {
					struct timespec timeout;
					if (!mlibc::time_absolute_to_relative(clock, abstime, &timeout))
						return EINVAL;
					if (timeout.tv_sec == 0 && timeout.tv_nsec == 0)
						return ETIMEDOUT;
				    e = mlibc::sysdep<FutexWait>(
				        (int *)&l->__mlibc_m,
				        m_expected | __mlibc_fair_rwlock::mutex_waiters_bit,
				        &timeout
				    );
			    } else {
				    e = mlibc::sysdep<FutexWait>(
				        (int *)&l->__mlibc_m,
				        m_expected | __mlibc_fair_rwlock::mutex_waiters_bit,
				        nullptr
				    );
			    }
				if (e != 0 && e != EAGAIN && e != EINTR)
					return e;

				// Opportunistically try to take the lock after we wake up.
				m_expected = 0;
			}else{
				// Try to lock the mutex.
				unsigned int desired = 1;
				if(excl)
					desired |= __mlibc_fair_rwlock::mutex_excl_bit;
			    if (l->__mlibc_m.compare_exchange_strong(
			            m_expected, desired, std::memory_order_acquire, std::memory_order_relaxed
			        ))
				    break;
			}
		}

		return 0;
	}

	int rwlock_m_trylock(pthread_rwlock_t *rw, bool excl) {
		auto l = __mlibc_fair_rwlock::from(rw);
		unsigned int m_expected = l->__mlibc_m.load(std::memory_order_relaxed);
		if(!m_expected) {
			// Try to lock the mutex.
			unsigned int desired = 1;
			if(excl)
				desired |= __mlibc_fair_rwlock::mutex_excl_bit;
		    if (l->__mlibc_m.compare_exchange_strong(
		            m_expected, desired, std::memory_order_acquire, std::memory_order_relaxed
		        ))
			    return 0;
		}

		__ensure(m_expected & __mlibc_fair_rwlock::mutex_owner_mask);

		// POSIX says that this function should never block but also that
		// readers should not be blocked by readers. We implement this by returning EAGAIN
		// (and not EBUSY) if a reader would block a reader.
		if(!excl && !(m_expected & __mlibc_fair_rwlock::mutex_excl_bit))
			return EAGAIN;

		return EBUSY;
	}

	void rwlock_m_unlock(pthread_rwlock_t *rw) {
		auto l = __mlibc_fair_rwlock::from(rw);
		auto m = l->__mlibc_m.exchange(0, std::memory_order_release);
		if(m & __mlibc_fair_rwlock::mutex_waiters_bit)
			mlibc::sysdep<FutexWake>((int *)&l->__mlibc_m, true);
	}
} // namespace

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
	attr->__heap_ptr = frg::construct<__mlibc_rwlockattr>(getAllocator());
	return 0;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict attr,
		int *__restrict pshared) {
	auto a = __mlibc_rwlockattr::from(attr);
	*pshared = a->__mlibc_pshared;
	return 0;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)
		return EINVAL;

	auto a = __mlibc_rwlockattr::from(attr);
	a->__mlibc_pshared = pshared;
	return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr) {
	if (attr && attr->__heap_ptr) {
		frg::destruct<__mlibc_rwlockattr>(
		    getAllocator(), __mlibc_rwlockattr::from(attr)
		);
		attr->__heap_ptr = nullptr;
	}
	return 0;
}

int pthread_rwlock_init(pthread_rwlock_t *__restrict rw, const pthread_rwlockattr_t *__restrict attr) {
	SCOPE_TRACE();
	auto lock = __mlibc_fair_rwlock::from(rw);
	auto a = attr ? __mlibc_rwlockattr::from(attr) : nullptr;
	new (lock) __mlibc_fair_rwlock((a && a->__mlibc_pshared == __MLIBC_THREAD_PROCESS_SHARED));
	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rw) {
	auto l = __mlibc_fair_rwlock::from(rw);
	__ensure(!l->__mlibc_m);
	__ensure(!l->__mlibc_rc);
	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	// Take the __mlibc_m mutex.
	// Will be released in pthread_rwlock_unlock().
	if(int e = rwlock_m_trylock(rw, true))
		return e;

	auto l = __mlibc_fair_rwlock::from(rw);
	// Check that there are no readers.
	unsigned int rc_expected = l->__mlibc_rc.load(std::memory_order_acquire);
	if(rc_expected) {
		rwlock_m_unlock(rw);
		return EBUSY;
	}

	return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	// Take the __mlibc_m mutex.
	// Will be released in pthread_rwlock_unlock().
	if (int e = rwlock_m_lock(rw, true); e)
		return e;

	auto l = __mlibc_fair_rwlock::from(rw);
	// Now wait until there are no more readers.
	unsigned int rc_expected = l->__mlibc_rc.load(std::memory_order_acquire);
	while(true) {
		if(!rc_expected)
			break;

		__ensure(rc_expected & __mlibc_fair_rwlock::rc_count_mask);

		// Try to set the waiters bit.
		if(!(rc_expected & __mlibc_fair_rwlock::rc_waiters_bit)) {
			unsigned int desired = rc_expected | __mlibc_fair_rwlock::rc_waiters_bit;
			if (!l->__mlibc_rc.compare_exchange_strong(
			        rc_expected, desired, std::memory_order_acquire, std::memory_order_acquire
			    ))
				continue;
		}

		// Wait on the futex.
		mlibc::sysdep<FutexWait>(
		    (int *)&l->__mlibc_rc, rc_expected | __mlibc_fair_rwlock::rc_waiters_bit, nullptr
		);

		// Re-check the reader counter.
		rc_expected = l->__mlibc_rc.load(std::memory_order_acquire);
	}

	return 0;
}

int pthread_rwlock_timedwrlock(pthread_rwlock_t *rw, const struct timespec *__restrict abstime) {
	return pthread_rwlock_clockwrlock(rw, CLOCK_REALTIME, abstime);
}

int pthread_rwlock_clockwrlock(pthread_rwlock_t *rw, clockid_t clock, const struct timespec *__restrict abstime) {
	SCOPE_TRACE();

	// POSIX: we must not fail with ETIMEDOUT if the lock can be taken immediately.
	if (int e = pthread_rwlock_trywrlock(rw); e != EBUSY)
		return e;

	// Take the __mlibc_m mutex.
	// Will be released in pthread_rwlock_unlock().
	if (int e = rwlock_m_lock(rw, true, clock, abstime); e)
		return e;
	frg::scope_exit unlockOnError{[&] { rwlock_m_unlock(rw); }};

	auto l = __mlibc_fair_rwlock::from(rw);
	// Now wait until there are no more readers.
	unsigned int rc_expected = l->__mlibc_rc.load(std::memory_order_acquire);
	while(true) {
		if(!rc_expected)
			break;

		__ensure(rc_expected & __mlibc_fair_rwlock::rc_count_mask);

		// Try to set the waiters bit.
		if(!(rc_expected & __mlibc_fair_rwlock::rc_waiters_bit)) {
			unsigned int desired = rc_expected | __mlibc_fair_rwlock::rc_waiters_bit;
			if (!l->__mlibc_rc.compare_exchange_strong(
			        rc_expected, desired, std::memory_order_acquire, std::memory_order_acquire
			    ))
				continue;
		}

		struct timespec timeout;
		if (!mlibc::time_absolute_to_relative(clock, abstime, &timeout))
			return EINVAL;
		if (timeout.tv_sec == 0 && timeout.tv_nsec == 0)
			return ETIMEDOUT;

		// Wait on the futex.
		int e = mlibc::sysdep<FutexWait>(
		    (int *)&l->__mlibc_rc, rc_expected | __mlibc_fair_rwlock::rc_waiters_bit, &timeout
		);
		if (e != 0 && e != EAGAIN && e != EINTR)
			return e;

		// Re-check the reader counter.
		rc_expected = l->__mlibc_rc.load(std::memory_order_acquire);
	}

	unlockOnError.release();
	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	// Increment the reader count while holding the __mlibc_m mutex.
	if(int e = rwlock_m_trylock(rw, false); e)
		return e;
	auto l = __mlibc_fair_rwlock::from(rw);
	l->__mlibc_rc.fetch_add(1, std::memory_order_acquire);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	// Increment the reader count while holding the __mlibc_m mutex.
	if (int e = rwlock_m_lock(rw, false); e)
		return e;
	auto l = __mlibc_fair_rwlock::from(rw);
	l->__mlibc_rc.fetch_add(1, std::memory_order_acquire);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_timedrdlock(pthread_rwlock_t *rw, const struct timespec *__restrict abstime) {
	return pthread_rwlock_clockrdlock(rw, CLOCK_REALTIME, abstime);
}

int pthread_rwlock_clockrdlock(pthread_rwlock_t *rw, clockid_t clock, const struct timespec *__restrict abstime) {
	SCOPE_TRACE();

	// POSIX: we must not fail with ETIMEDOUT if the lock can be taken immediately.
	if (int e = pthread_rwlock_tryrdlock(rw); e != EBUSY)
		return e;

	// Increment the reader count while holding the __mlibc_m mutex.
	if (int e = rwlock_m_lock(rw, false, clock, abstime); e)
		return e;

	auto l = __mlibc_fair_rwlock::from(rw);
	l->__mlibc_rc.fetch_add(1, std::memory_order_acquire);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	auto l = __mlibc_fair_rwlock::from(rw);
	unsigned int rc_expected = l->__mlibc_rc.load(std::memory_order_relaxed);
	if(!rc_expected) {
		// We are doing a write-unlock.
		rwlock_m_unlock(rw);
		return 0;
	}else{
		// We are doing a read-unlock.
		while(true) {
			unsigned int count = rc_expected & __mlibc_fair_rwlock::rc_count_mask;
			__ensure(count);

			// Try to decrement the count.
			if(count == 1 && (rc_expected & __mlibc_fair_rwlock::rc_waiters_bit)) {
				unsigned int desired = 0;
				if (!l->__mlibc_rc.compare_exchange_weak(
				        rc_expected, desired, std::memory_order_release, std::memory_order_relaxed
				    ))
					continue;

				// Wake the futex.
				mlibc::sysdep<FutexWake>((int *)&l->__mlibc_rc, true);
				break;
			}else{
				unsigned int desired = (rc_expected & ~__mlibc_fair_rwlock::rc_count_mask) | (count - 1);
				if (!l->__mlibc_rc.compare_exchange_weak(
				        rc_expected, desired, std::memory_order_release, std::memory_order_relaxed
				    ))
					continue;
				break;
			}
		}

		return 0;
	}
}

int pthread_getcpuclockid(pthread_t, clockid_t *) {
	mlibc::infoLogger() << "mlibc: pthread_getcpuclockid() always returns ENOENT"
			<< frg::endlog;
	return ENOENT;
}

int pthread_spin_init(pthread_spinlock_t *lock, int) {
	auto l = __mlibc_spinlock::from(lock);
	new (l) __mlibc_spinlock();
	return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *) {
	return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock) {
	unsigned int desired = mlibc::this_tid();
	unsigned int expected = 0;
	auto l = __mlibc_spinlock::from(lock);

	while (true) {
		if (l->__lock.compare_exchange_weak(
		        expected, desired, std::memory_order_acquire, std::memory_order_relaxed
		    ))
			break;
		if (expected == desired)
			return EDEADLK;

		while (l->__lock.load(std::memory_order_relaxed) != 0)
			frg::detail::loophint();
		expected = 0;
	}

	return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock) {
	unsigned int desired = mlibc::this_tid();
	unsigned int expected = 0;
	auto l = __mlibc_spinlock::from(lock);

	if (!l->__lock.compare_exchange_strong(
	        expected, desired, std::memory_order_acquire, std::memory_order_relaxed
	    ))
		return EBUSY;

	return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock) {
	auto l = __mlibc_spinlock::from(lock);
	auto val = l->__lock.load(std::memory_order_relaxed);
	if (val != mlibc::this_tid())
		return EPERM;
	l->__lock.store(0, std::memory_order_release);
	return 0;
}

int pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_rwlockattr_getkind_np(const pthread_rwlockattr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
