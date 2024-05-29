
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>

#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <frg/array.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/tid.hpp>
#include <mlibc/threads.hpp>

static bool enableTrace = false;

struct ScopeTrace {
	ScopeTrace(const char *file, int line, const char *function)
	: _file(file), _line(line), _function(function) {
		if(!enableTrace)
			return;
		mlibc::infoLogger() << "trace: Enter scope "
				<< _file << ":" << _line << " (in function "
				<< _function << ")" << frg::endlog;
	}

	~ScopeTrace() {
		if(!enableTrace)
			return;
		mlibc::infoLogger() << "trace: Exit scope" << frg::endlog;
	}

private:
	const char *_file;
	int _line;
	const char *_function;
};

#define SCOPE_TRACE() ScopeTrace(__FILE__, __LINE__, __FUNCTION__)

static constexpr unsigned int mutexRecursive = 1;

// TODO: either use uint32_t or determine the bit based on sizeof(int).
static constexpr unsigned int mutex_owner_mask = (static_cast<uint32_t>(1) << 30) - 1;
static constexpr unsigned int mutex_waiters_bit = static_cast<uint32_t>(1) << 31;

// Only valid for the internal __mlibc_m mutex of wrlocks.
static constexpr unsigned int mutex_excl_bit = static_cast<uint32_t>(1) << 30;

static constexpr unsigned int rc_count_mask = (static_cast<uint32_t>(1) << 31) - 1;
static constexpr unsigned int rc_waiters_bit = static_cast<uint32_t>(1) << 31;

static constexpr size_t default_stacksize = 0x200000;
static constexpr size_t default_guardsize = 4096;

// ----------------------------------------------------------------------------
// pthread_attr and pthread functions.
// ----------------------------------------------------------------------------

// pthread_attr functions.
int pthread_attr_init(pthread_attr_t *attr) {
	*attr = pthread_attr_t{};
	attr->__mlibc_stacksize = default_stacksize;
	attr->__mlibc_guardsize = default_guardsize;
	attr->__mlibc_detachstate = PTHREAD_CREATE_JOINABLE;
	return 0;
}

int pthread_attr_destroy(pthread_attr_t *) {
	return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate) {
	*detachstate = attr->__mlibc_detachstate;
	return 0;
}
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate) {
	if (detachstate != PTHREAD_CREATE_DETACHED &&
			detachstate != PTHREAD_CREATE_JOINABLE)
		return EINVAL;

	attr->__mlibc_detachstate = detachstate;
	return 0;
}

int pthread_attr_getstacksize(const pthread_attr_t *__restrict attr, size_t *__restrict stacksize) {
	*stacksize = attr->__mlibc_stacksize;
	return 0;
}

int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
	if (stacksize < PTHREAD_STACK_MIN)
		return EINVAL;
	attr->__mlibc_stacksize = stacksize;
	return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
	*stackaddr = attr->__mlibc_stackaddr;
	return 0;
}
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
	attr->__mlibc_stackaddr = stackaddr;
	return 0;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
	*stackaddr = attr->__mlibc_stackaddr;
	*stacksize = attr->__mlibc_stacksize;
	return 0;
}
int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
	if (stacksize < PTHREAD_STACK_MIN)
		return EINVAL;
	attr->__mlibc_stacksize = stacksize;
	attr->__mlibc_stackaddr = stackaddr;
	return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t *__restrict attr, size_t *__restrict guardsize) {
	*guardsize = attr->__mlibc_guardsize;
	return 0;
}
int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize) {
	attr->__mlibc_guardsize = guardsize;
	return 0;
}

int pthread_attr_getscope(const pthread_attr_t *attr, int *scope) {
	*scope = attr->__mlibc_scope;
	return 0;
}
int pthread_attr_setscope(pthread_attr_t *attr, int scope) {
	if (scope != PTHREAD_SCOPE_SYSTEM &&
			scope != PTHREAD_SCOPE_PROCESS)
		return EINVAL;
	if (scope == PTHREAD_SCOPE_PROCESS)
		return ENOTSUP;
	attr->__mlibc_scope = scope;
	return 0;
}

int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
	*inheritsched = attr->__mlibc_inheritsched;
	return 0;
}
int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
	if (inheritsched != PTHREAD_INHERIT_SCHED &&
			inheritsched != PTHREAD_EXPLICIT_SCHED)
		return EINVAL;
	attr->__mlibc_inheritsched = inheritsched;
	return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t *__restrict attr, struct sched_param *__restrict schedparam) {
	*schedparam = attr->__mlibc_schedparam;
	return 0;
}
int pthread_attr_setschedparam(pthread_attr_t *__restrict attr, const struct sched_param *__restrict schedparam) {
	// TODO: this is supposed to return EINVAL for when the schedparam doesn't make sense
	// for the given schedpolicy.
	attr->__mlibc_schedparam = *schedparam;
	return 0;
}

int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict attr, int *__restrict policy) {
	*policy = attr->__mlibc_schedpolicy;
	return 0;
}
int pthread_attr_setschedpolicy(pthread_attr_t *__restrict attr, int policy) {
	if (policy != SCHED_FIFO && policy != SCHED_RR &&
			policy != SCHED_OTHER)
		return EINVAL;
	attr->__mlibc_schedpolicy = policy;
	return 0;
}

#if __MLIBC_LINUX_OPTION
int pthread_attr_getaffinity_np(const pthread_attr_t *__restrict attr,
		size_t cpusetsize, cpu_set_t *__restrict cpusetp) {
	if (!attr)
		return EINVAL;

	if (!attr->__mlibc_cpuset) {
		memset(cpusetp, -1, cpusetsize);
		return 0;
	}

	for (size_t cnt = cpusetsize; cnt < attr->__mlibc_cpusetsize; cnt++)
		if (reinterpret_cast<char*>(attr->__mlibc_cpuset)[cnt] != '\0')
			return ERANGE;

	auto p = memcpy(cpusetp, attr->__mlibc_cpuset,
			std::min(cpusetsize, attr->__mlibc_cpusetsize));
	if (cpusetsize > attr->__mlibc_cpusetsize)
		memset(p, '\0', cpusetsize - attr->__mlibc_cpusetsize);

	return 0;
}

int pthread_attr_setaffinity_np(pthread_attr_t *__restrict attr,
		size_t cpusetsize, const cpu_set_t *__restrict cpusetp) {
	if (!attr)
		return EINVAL;

	if (!cpusetp || !cpusetsize) {
		attr->__mlibc_cpuset = NULL;
		attr->__mlibc_cpusetsize = 0;
		return 0;
	}

	if (attr->__mlibc_cpusetsize != cpusetsize) {
		auto newp = realloc(attr->__mlibc_cpuset, cpusetsize);
		if (!newp)
			return ENOMEM;

		attr->__mlibc_cpuset = static_cast<cpu_set_t*>(newp);
		attr->__mlibc_cpusetsize = cpusetsize;
	}

	memcpy(attr->__mlibc_cpuset, cpusetp, cpusetsize);
	return 0;
}

int pthread_attr_getsigmask_np(const pthread_attr_t *__restrict attr,
		sigset_t *__restrict sigmask) {
	if (!attr)
		return EINVAL;

	if (!attr->__mlibc_sigmaskset) {
		sigemptyset(sigmask);
		return PTHREAD_ATTR_NO_SIGMASK_NP;
	}

	*sigmask = attr->__mlibc_sigmask;

	return 0;
}
int pthread_attr_setsigmask_np(pthread_attr_t *__restrict attr,
		const sigset_t *__restrict sigmask) {
	if (!attr)
		return EINVAL;

	if (!sigmask) {
		attr->__mlibc_sigmaskset = 0;
		return 0;
	}

	attr->__mlibc_sigmask = *sigmask;
	attr->__mlibc_sigmaskset = 1;

	// Filter out internally used signals.
	sigdelset(&attr->__mlibc_sigmask, SIGCANCEL);

	return 0;
}

namespace {
	void get_own_stackinfo(void **stack_addr, size_t *stack_size) {
		auto fp = fopen("/proc/self/maps", "r");
		if (!fp) {
			mlibc::infoLogger() << "mlibc pthreads: /proc/self/maps does not exist! Producing incorrect"
				" stack results!" << frg::endlog;
			return;
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
				return;
			}
		}

		fclose(fp);
	}
}

int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	*attr = pthread_attr_t{};

	if (!tcb->stackAddr || !tcb->stackSize) {
		get_own_stackinfo(&attr->__mlibc_stackaddr, &attr->__mlibc_stacksize);
	} else {
		attr->__mlibc_stacksize = tcb->stackSize;
		attr->__mlibc_stackaddr = tcb->stackAddr;
	}

	attr->__mlibc_guardsize = tcb->guardSize;
	attr->__mlibc_detachstate = tcb->isJoinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED;
	mlibc::infoLogger() << "pthread_getattr_np(): Implementation is incomplete!" << frg::endlog;
	return 0;
}

int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *mask) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getthreadaffinity, ENOSYS);
	return mlibc::sys_getthreadaffinity(reinterpret_cast<Tcb*>(thread)->tid, cpusetsize, mask);
}

int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *mask) {
	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setthreadaffinity, ENOSYS);
	return mlibc::sys_setthreadaffinity(reinterpret_cast<Tcb*>(thread)->tid, cpusetsize, mask);
}
#endif // __MLIBC_LINUX_OPTION

extern "C" Tcb *__rtld_allocateTcb();

// pthread functions.
int pthread_create(pthread_t *__restrict thread, const pthread_attr_t *__restrict attrp,
		void *(*entry) (void *), void *__restrict user_arg) {
	return mlibc::thread_create(thread, attrp, reinterpret_cast<void *>(entry), user_arg, false);
}

pthread_t pthread_self(void) {
	return reinterpret_cast<pthread_t>(mlibc::get_current_tcb());
}

int pthread_equal(pthread_t t1, pthread_t t2) {
	if(t1 == t2)
		return 1;
	return 0;
}

namespace {
	struct key_global_info {
		bool in_use;

		void (*dtor)(void *);
		uint64_t generation;
	};

	constinit frg::array<
		key_global_info,
		PTHREAD_KEYS_MAX
	> key_globals_{};

	FutexLock key_mutex_;
}

namespace mlibc {
	__attribute__ ((__noreturn__)) void do_exit() {
		sys_thread_exit();
		__builtin_unreachable();
	}
}

__attribute__ ((__noreturn__)) void pthread_exit(void *ret_val) {
	auto self = mlibc::get_current_tcb();

	if (__atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED) & tcbExitingBit)
		mlibc::do_exit();

	__atomic_fetch_or(&self->cancelBits, tcbExitingBit, __ATOMIC_RELAXED);

	auto hand = self->cleanupEnd;
	while (hand) {
		auto old = hand;
		hand->func(hand->arg);
		hand = hand->prev;
		frg::destruct(getAllocator(), old);
	}

	for (size_t j = 0; j < PTHREAD_DESTRUCTOR_ITERATIONS; j++) {
		for (size_t i = 0; i < PTHREAD_KEYS_MAX; i++) {
			if (auto v = pthread_getspecific(i)) {
				key_mutex_.lock();
				auto dtor = key_globals_[i].dtor;
				key_mutex_.unlock();

				if (dtor) {
					dtor(v);
					(*self->localKeys)[i].value = nullptr;
				}
			}
		}
	}

	self->returnValue.voidPtr = ret_val;
	__atomic_store_n(&self->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&self->didExit);

	// TODO: clean up thread resources when we are detached.

	// TODO: do exit(0) when we're the only thread instead
	mlibc::do_exit();
}

int pthread_join(pthread_t thread, void **ret) {
	return mlibc::thread_join(thread, ret);
}

int pthread_detach(pthread_t thread) {
	auto tcb = reinterpret_cast<Tcb*>(thread);
	if (!__atomic_load_n(&tcb->isJoinable, __ATOMIC_RELAXED))
		return EINVAL;

	int expected = 1;
	if(!__atomic_compare_exchange_n(&tcb->isJoinable, &expected, 0, false, __ATOMIC_RELEASE,
				__ATOMIC_RELAXED))
		return EINVAL;

	return 0;
}

void pthread_cleanup_push(void (*func) (void *), void *arg) {
	auto self = mlibc::get_current_tcb();

	auto hand = frg::construct<Tcb::CleanupHandler>(getAllocator());
	hand->func = func;
	hand->arg = arg;
	hand->next = nullptr;
	hand->prev = self->cleanupEnd;

	if (self->cleanupEnd)
		self->cleanupEnd->next = hand;

	self->cleanupEnd = hand;

	if (!self->cleanupBegin)
		self->cleanupBegin = self->cleanupEnd;
}

void pthread_cleanup_pop(int execute) {
	auto self = mlibc::get_current_tcb();

	auto hand = self->cleanupEnd;

	if (self->cleanupEnd)
		self->cleanupEnd = self->cleanupEnd->prev;
	if (self->cleanupEnd)
		self->cleanupEnd->next = nullptr;

	if (execute)
		hand->func(hand->arg);

	frg::destruct(getAllocator(), hand);
}

int pthread_setname_np(pthread_t thread, const char *name) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_thread_setname, ENOSYS);
	if(int e = sysdep(tcb, name); e) {
		return e;
	}

	return 0;
}

int pthread_getname_np(pthread_t thread, char *name, size_t size) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	auto sysdep = MLIBC_CHECK_OR_ENOSYS(mlibc::sys_thread_getname, ENOSYS);
	if(int e = sysdep(tcb, name, size); e) {
		return e;
	}

	return 0;
}

int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_setschedparam, ENOSYS);
	if(int e = mlibc::sys_setschedparam(tcb, policy, param); e) {
		return e;
	}

	return 0;
}

int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
	auto tcb = reinterpret_cast<Tcb*>(thread);

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_getschedparam, ENOSYS);
	if(int e = mlibc::sys_getschedparam(tcb, policy, param); e) {
		return e;
	}

	return 0;
}

//pthread cancel functions

extern "C" void __mlibc_do_cancel() {
	//TODO(geert): for now the same as pthread_exit()
	pthread_exit(PTHREAD_CANCELED);
}

namespace {

	void sigcancel_handler(int signal, siginfo_t *info, void *ucontext) {
		ucontext_t *uctx = static_cast<ucontext_t*>(ucontext);
		// The function could be called from other signals, or from another
		// process, in which case we should do nothing.
		if (signal != SIGCANCEL || info->si_pid != getpid() ||
				info->si_code != SI_TKILL)
			return;

		auto tcb = reinterpret_cast<Tcb*>(mlibc::get_current_tcb());
		int old_value = tcb->cancelBits;

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
				mlibc::sys_before_cancellable_syscall && !mlibc::sys_before_cancellable_syscall(uctx))
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
}

namespace mlibc {
namespace {

struct PthreadSignalInstaller {
	PthreadSignalInstaller() {
		struct sigaction sa;
		sa.sa_sigaction = sigcancel_handler;
		sa.sa_flags = SA_SIGINFO;
		auto e = ENOSYS;
		if(sys_sigaction)
			e = sys_sigaction(SIGCANCEL, &sa, NULL);
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

	auto self = reinterpret_cast<Tcb *>(mlibc::get_current_tcb());
	int old_value = self->cancelBits;
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

	auto self = reinterpret_cast<Tcb *>(mlibc::get_current_tcb());
	int old_value = self->cancelBits;
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
			if (new_value & PTHREAD_CANCEL_ENABLE)
				sigprocmask(SIG_UNBLOCK, &set, NULL);
			else
				sigprocmask(SIG_BLOCK, &set, NULL);
			break;
		}

		old_value = current_value;
	}

	return 0;
}
void pthread_testcancel(void) {
	auto self = reinterpret_cast<Tcb *>(mlibc::get_current_tcb());
	int value = self->cancelBits;
	if ((value & tcbCancelEnableBit) && (value & tcbCancelTriggerBit)) {
		__mlibc_do_cancel();
		__builtin_unreachable();
	}
}
int pthread_cancel(pthread_t thread) {
	if (!mlibc::sys_tgkill) {
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
				pid_t pid = getpid();

				int res = mlibc::sys_tgkill(pid, tcb->tid, SIGCANCEL);

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
		return -1;

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

	auto g = frg::guard(&key_mutex_);

	pthread_key_t key = PTHREAD_KEYS_MAX;
	for (size_t i = 0; i < PTHREAD_KEYS_MAX; i++) {
		if (!key_globals_[i].in_use) {
			key = i;
			break;
		}
	}

	if (key == PTHREAD_KEYS_MAX)
		return EAGAIN;

	key_globals_[key].in_use = true;
	key_globals_[key].dtor = destructor;

	*out = key;

	return 0;
}

int pthread_key_delete(pthread_key_t key) {
	SCOPE_TRACE();

	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return EINVAL;

	key_globals_[key].in_use = false;
	key_globals_[key].dtor = nullptr;
	key_globals_[key].generation++;

	return 0;
}

void *pthread_getspecific(pthread_key_t key) {
	SCOPE_TRACE();

	auto self = mlibc::get_current_tcb();
	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return nullptr;

	if (key_globals_[key].generation > (*self->localKeys)[key].generation) {
		(*self->localKeys)[key].value = nullptr;
		(*self->localKeys)[key].generation = key_globals_[key].generation;
	}

	return (*self->localKeys)[key].value;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
	SCOPE_TRACE();

	auto self = mlibc::get_current_tcb();
	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return EINVAL;

	(*self->localKeys)[key].value = const_cast<void *>(value);
	(*self->localKeys)[key].generation = key_globals_[key].generation;

	return 0;
}

// ----------------------------------------------------------------------------
// pthread_once functions.
// ----------------------------------------------------------------------------

static constexpr unsigned int onceComplete = 1;
static constexpr unsigned int onceLocked = 2;

int pthread_once(pthread_once_t *once, void (*function) (void)) {
	SCOPE_TRACE();

	auto expected = __atomic_load_n(&once->__mlibc_done, __ATOMIC_ACQUIRE);

	// fast path: the function was already run.
	while(!(expected & onceComplete)) {
		if(!expected) {
			// try to acquire the mutex.
			if(!__atomic_compare_exchange_n(&once->__mlibc_done,
					&expected, onceLocked, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
				continue;

			function();

			// unlock the mutex.
			__atomic_exchange_n(&once->__mlibc_done, onceComplete, __ATOMIC_RELEASE);
			if(int e = mlibc::sys_futex_wake((int *)&once->__mlibc_done); e)
				__ensure(!"sys_futex_wake() failed");
			return 0;
		}else{
			// a different thread is currently running the initializer.
			__ensure(expected == onceLocked);
			// if the wait gets interrupted by a signal, check again.
			// EAGAIN will also be a retry, as it means the other thread completed
			// and changed the __mlibc_done variable to signal it before we actually went to sleep.
			if(int e = mlibc::sys_futex_wait((int *)&once->__mlibc_done, onceLocked, nullptr); e && e != EINTR && e != EAGAIN)
				__ensure(!"sys_futex_wait() failed");
			expected =  __atomic_load_n(&once->__mlibc_done, __ATOMIC_ACQUIRE);
		}
	}

	return 0;
}

// ----------------------------------------------------------------------------
// pthread_mutexattr and pthread_mutex functions.
// ----------------------------------------------------------------------------

// pthread_mutexattr functions
int pthread_mutexattr_init(pthread_mutexattr_t *attr) {
	SCOPE_TRACE();
	return mlibc::thread_mutexattr_init(attr);
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *attr) {
	SCOPE_TRACE();
	return mlibc::thread_mutexattr_destroy(attr);
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict attr, int *__restrict type) {
	return mlibc::thread_mutexattr_gettype(attr, type);
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	return mlibc::thread_mutexattr_settype(attr, type);
}

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict attr,
		int *__restrict robust) {
	*robust = attr->__mlibc_robust;
	return 0;
}
int pthread_mutexattr_setrobust(pthread_mutexattr_t *attr, int robust) {
	if (robust != PTHREAD_MUTEX_STALLED && robust != PTHREAD_MUTEX_ROBUST)
		return EINVAL;

	attr->__mlibc_robust = robust;
	return 0;
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared) {
	*pshared = attr->__mlibc_pshared;
	return 0;
}
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	attr->__mlibc_pshared = pshared;
	return 0;
}

int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *__restrict attr,
		int *__restrict protocol) {
	*protocol = attr->__mlibc_protocol;
	return 0;
}

int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol) {
	if (protocol != PTHREAD_PRIO_NONE && protocol != PTHREAD_PRIO_INHERIT
			&& protocol != PTHREAD_PRIO_PROTECT)
		return EINVAL;

	attr->__mlibc_protocol = protocol;
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

	return mlibc::thread_mutex_init(mutex, attr);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	return mlibc::thread_mutex_destroy(mutex);
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_lock(mutex);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	unsigned int this_tid = mlibc::this_tid();
	unsigned int expected = __atomic_load_n(&mutex->__mlibc_state, __ATOMIC_RELAXED);
	if(!expected) {
		// Try to take the mutex here.
		if(__atomic_compare_exchange_n(&mutex->__mlibc_state,
						&expected, this_tid, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
			__ensure(!mutex->__mlibc_recursion);
			mutex->__mlibc_recursion = 1;
			return 0;
		}
	} else {
		// If this (recursive) mutex is already owned by us, increment the recursion level.
		if((expected & mutex_owner_mask) == this_tid) {
			if(!(mutex->__mlibc_flags & mutexRecursive)) {
				return EBUSY;
			}
			++mutex->__mlibc_recursion;
			return 0;
		}
	}

	return EBUSY;
}

int pthread_mutex_timedlock(pthread_mutex_t *__restrict,
		const struct timespec *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	return mlibc::thread_mutex_unlock(mutex);
}

int pthread_mutex_consistent(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// pthread_condattr and pthread_cond functions.
// ----------------------------------------------------------------------------

int pthread_condattr_init(pthread_condattr_t *attr) {
	attr->__mlibc_pshared = PTHREAD_PROCESS_PRIVATE;
	attr->__mlibc_clock = CLOCK_REALTIME;
	return 0;
}

int pthread_condattr_destroy(pthread_condattr_t *attr) {
	memset(attr, 0, sizeof(*attr));
	return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *__restrict attr,
		clockid_t *__restrict clock) {
	*clock = attr->__mlibc_clock;
	return 0;
}

int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock) {
	if (clock != CLOCK_REALTIME && clock != CLOCK_MONOTONIC
			&& clock != CLOCK_MONOTONIC_RAW && clock != CLOCK_REALTIME_COARSE
			&& clock != CLOCK_MONOTONIC_COARSE && clock != CLOCK_BOOTTIME)
		return EINVAL;

	attr->__mlibc_clock = clock;
	return 0;
}

int pthread_condattr_getpshared(const pthread_condattr_t *__restrict attr,
		int *__restrict pshared) {
	*pshared = attr->__mlibc_pshared;
	return 0;
}

int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_PRIVATE && pshared != PTHREAD_PROCESS_SHARED)
		return EINVAL;

	attr->__mlibc_pshared = pshared;
	return 0;
}

int pthread_cond_init(pthread_cond_t *__restrict cond, const pthread_condattr_t *__restrict attr) {
	SCOPE_TRACE();

	return mlibc::thread_cond_init(cond, attr);
}

int pthread_cond_destroy(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return mlibc::thread_cond_destroy(cond);
}

int pthread_cond_wait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex) {
	return pthread_cond_timedwait(cond, mutex, nullptr);
}

int pthread_cond_timedwait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex,
		const struct timespec *__restrict abstime) {
	return mlibc::thread_cond_timedwait(cond, mutex, abstime);
}

int pthread_cond_signal(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return pthread_cond_broadcast(cond);
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return mlibc::thread_cond_broadcast(cond);
}

// ----------------------------------------------------------------------------
// pthread_barrierattr and pthread_barrier functions.
// ----------------------------------------------------------------------------

int pthread_barrierattr_init(pthread_barrierattr_t *attr) {
	attr->__mlibc_pshared = PTHREAD_PROCESS_PRIVATE;
	return 0;
}

int pthread_barrierattr_getpshared(const pthread_barrierattr_t *__restrict attr,
		int *__restrict pshared) {
	*pshared = attr->__mlibc_pshared;
	return 0;
}

int pthread_barrierattr_setpshared(pthread_barrierattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)
		return EINVAL;

	attr->__mlibc_pshared = pshared;
	return 0;
}

int pthread_barrierattr_destroy(pthread_barrierattr_t *) {
	return 0;
}

int pthread_barrier_init(pthread_barrier_t *__restrict barrier,
		const pthread_barrierattr_t *__restrict attr, unsigned count) {
	if (count == 0)
		return EINVAL;

	barrier->__mlibc_waiting = 0;
	barrier->__mlibc_inside = 0;
	barrier->__mlibc_seq = 0;
	barrier->__mlibc_count = count;

	// Since we don't implement these yet, set a flag to error later.
	auto pshared = attr ? attr->__mlibc_pshared : PTHREAD_PROCESS_PRIVATE;
	barrier->__mlibc_flags = pshared;

	return 0;
}

int pthread_barrier_destroy(pthread_barrier_t *barrier) {
	// Wait until there are no threads still using the barrier.
	unsigned inside = 0;
	do {
		unsigned expected = __atomic_load_n(&barrier->__mlibc_inside, __ATOMIC_RELAXED);
		if (expected == 0)
			break;

		int e = mlibc::sys_futex_wait((int *)&barrier->__mlibc_inside, expected, nullptr);
		if (e != 0 && e != EAGAIN && e != EINTR)
			mlibc::panicLogger() << "mlibc: sys_futex_wait() returned error " << e << frg::endlog;
	} while (inside > 0);

	memset(barrier, 0, sizeof *barrier);
	return 0;
}

int pthread_barrier_wait(pthread_barrier_t *barrier) {
	if (barrier->__mlibc_flags != 0) {
		mlibc::panicLogger() << "mlibc: pthread_barrier_t flags were non-zero"
			<< frg::endlog;
	}

	// inside is incremented on entry and decremented on exit.
	// This is used to synchronise with pthread_barrier_destroy, to ensure that a thread doesn't pass
	// the barrier and immediately destroy its state while other threads still rely on it.

	__atomic_fetch_add(&barrier->__mlibc_inside, 1, __ATOMIC_ACQUIRE);

	auto leave = [&](){
		unsigned inside = __atomic_sub_fetch(&barrier->__mlibc_inside, 1, __ATOMIC_RELEASE);
		if (inside == 0)
			mlibc::sys_futex_wake((int *)&barrier->__mlibc_inside);
	};

	unsigned seq = __atomic_load_n(&barrier->__mlibc_seq, __ATOMIC_ACQUIRE);

	while (true) {
		unsigned expected = __atomic_load_n(&barrier->__mlibc_waiting, __ATOMIC_RELAXED);
		bool swapped = __atomic_compare_exchange_n(&barrier->__mlibc_waiting, &expected, expected + 1, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE);

		if (swapped) {
			if (expected + 1 == barrier->__mlibc_count) {
				// We were the last thread to hit the barrier. Reset waiters and wake the others.
				__atomic_fetch_add(&barrier->__mlibc_seq, 1, __ATOMIC_ACQUIRE);
				__atomic_store_n(&barrier->__mlibc_waiting, 0, __ATOMIC_RELEASE);

				mlibc::sys_futex_wake((int *)&barrier->__mlibc_seq);

				leave();
				return PTHREAD_BARRIER_SERIAL_THREAD;
			}

			while (true) {
				int e = mlibc::sys_futex_wait((int *)&barrier->__mlibc_seq, seq, nullptr);
				if (e != 0 && e != EAGAIN && e != EINTR)
					mlibc::panicLogger() << "mlibc: sys_futex_wait() returned error " << e << frg::endlog;

				unsigned newSeq = __atomic_load_n(&barrier->__mlibc_seq, __ATOMIC_ACQUIRE);
				if (newSeq > seq) {
					leave();
					return 0;
				}
			}
		}
	}
}

// ----------------------------------------------------------------------------
// pthread_rwlock functions.
// ----------------------------------------------------------------------------

namespace {
	void rwlock_m_lock(pthread_rwlock_t *rw, bool excl) {
		unsigned int m_expected = __atomic_load_n(&rw->__mlibc_m, __ATOMIC_RELAXED);
		while(true) {
			if(m_expected) {
				__ensure(m_expected & mutex_owner_mask);

				// Try to set the waiters bit.
				if(!(m_expected & mutex_waiters_bit)) {
					unsigned int desired = m_expected | mutex_waiters_bit;
					if(!__atomic_compare_exchange_n(&rw->__mlibc_m,
							&m_expected, desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
						continue;
				}

				// Wait on the futex.
				mlibc::sys_futex_wait((int *)&rw->__mlibc_m, m_expected | mutex_waiters_bit, nullptr);

				// Opportunistically try to take the lock after we wake up.
				m_expected = 0;
			}else{
				// Try to lock the mutex.
				unsigned int desired = 1;
				if(excl)
					desired |= mutex_excl_bit;
				if(__atomic_compare_exchange_n(&rw->__mlibc_m,
						&m_expected, desired, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
					break;
			}
		}
	}

	int rwlock_m_trylock(pthread_rwlock_t *rw, bool excl) {
		unsigned int m_expected = __atomic_load_n(&rw->__mlibc_m, __ATOMIC_RELAXED);
		if(!m_expected) {
			// Try to lock the mutex.
			unsigned int desired = 1;
			if(excl)
				desired |= mutex_excl_bit;
			if(__atomic_compare_exchange_n(&rw->__mlibc_m,
					&m_expected, desired, false, __ATOMIC_ACQUIRE, __ATOMIC_RELAXED))
				return 0;
		}

		__ensure(m_expected & mutex_owner_mask);

		// POSIX says that this function should never block but also that
		// readers should not be blocked by readers. We implement this by returning EAGAIN
		// (and not EBUSY) if a reader would block a reader.
		if(!excl && !(m_expected & mutex_excl_bit))
			return EAGAIN;

		return EBUSY;
	}

	void rwlock_m_unlock(pthread_rwlock_t *rw) {
		auto m = __atomic_exchange_n(&rw->__mlibc_m, 0, __ATOMIC_RELEASE);
		if(m & mutex_waiters_bit)
			mlibc::sys_futex_wake((int *)&rw->__mlibc_m);
	}
}

int pthread_rwlockattr_init(pthread_rwlockattr_t *attr) {
	attr->__mlibc_pshared = PTHREAD_PROCESS_PRIVATE;
	return 0;
}

int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *__restrict attr,
		int *__restrict pshared) {
	*pshared = attr->__mlibc_pshared;
	return 0;
}

int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int pshared) {
	if (pshared != PTHREAD_PROCESS_SHARED && pshared != PTHREAD_PROCESS_PRIVATE)
		return EINVAL;

	attr->__mlibc_pshared = pshared;
	return 0;
}

int pthread_rwlockattr_destroy(pthread_rwlockattr_t *) {
	return 0;
}

int pthread_rwlock_init(pthread_rwlock_t *__restrict rw, const pthread_rwlockattr_t *__restrict attr) {
	SCOPE_TRACE();
	rw->__mlibc_m = 0;
	rw->__mlibc_rc = 0;

	// Since we don't implement this yet, set a flag to error later.
	auto pshared = attr ? attr->__mlibc_pshared : PTHREAD_PROCESS_PRIVATE;
	rw->__mlibc_flags = pshared;
	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rw) {
	__ensure(!rw->__mlibc_m);
	__ensure(!rw->__mlibc_rc);
	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	if (rw->__mlibc_flags != 0) {
		mlibc::panicLogger() << "mlibc: pthread_rwlock_t flags were non-zero"
			<< frg::endlog;
	}

	// Take the __mlibc_m mutex.
	// Will be released in pthread_rwlock_unlock().
	if(int e = rwlock_m_trylock(rw, true))
		return e;

	// Check that there are no readers.
	unsigned int rc_expected = __atomic_load_n(&rw->__mlibc_rc, __ATOMIC_ACQUIRE);
	if(rc_expected) {
		rwlock_m_unlock(rw);
		return EBUSY;
	}

	return 0;
}

int pthread_rwlock_wrlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	if (rw->__mlibc_flags != 0) {
		mlibc::panicLogger() << "mlibc: pthread_rwlock_t flags were non-zero"
			<< frg::endlog;
	}

	// Take the __mlibc_m mutex.
	// Will be released in pthread_rwlock_unlock().
	rwlock_m_lock(rw, true);

	// Now wait until there are no more readers.
	unsigned int rc_expected = __atomic_load_n(&rw->__mlibc_rc, __ATOMIC_ACQUIRE);
	while(true) {
		if(!rc_expected)
			break;

		__ensure(rc_expected & rc_count_mask);

		// Try to set the waiters bit.
		if(!(rc_expected & rc_waiters_bit)) {
			unsigned int desired = rc_expected | rc_count_mask;
			if(!__atomic_compare_exchange_n(&rw->__mlibc_rc,
					&rc_expected, desired, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
				continue;
		}

		// Wait on the futex.
		mlibc::sys_futex_wait((int *)&rw->__mlibc_rc, rc_expected | rc_waiters_bit, nullptr);

		// Re-check the reader counter.
		rc_expected = __atomic_load_n(&rw->__mlibc_rc, __ATOMIC_ACQUIRE);
	}

	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	if (rw->__mlibc_flags != 0) {
		mlibc::panicLogger() << "mlibc: pthread_rwlock_t flags were non-zero"
			<< frg::endlog;
	}

	// Increment the reader count while holding the __mlibc_m mutex.
	if(int e = rwlock_m_trylock(rw, false); e)
		return e;
	__atomic_fetch_add(&rw->__mlibc_rc, 1, __ATOMIC_ACQUIRE);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	if (rw->__mlibc_flags != 0) {
		mlibc::panicLogger() << "mlibc: pthread_rwlock_t flags were non-zero"
			<< frg::endlog;
	}

	// Increment the reader count while holding the __mlibc_m mutex.
	rwlock_m_lock(rw, false);
	__atomic_fetch_add(&rw->__mlibc_rc, 1, __ATOMIC_ACQUIRE);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

	unsigned int rc_expected = __atomic_load_n(&rw->__mlibc_rc, __ATOMIC_RELAXED);
	if(!rc_expected) {
		// We are doing a write-unlock.
		rwlock_m_unlock(rw);
		return 0;
	}else{
		// We are doing a read-unlock.
		while(true) {
			unsigned int count = rc_expected & rc_count_mask;
			__ensure(count);

			// Try to decrement the count.
			if(count == 1 && (rc_expected & rc_waiters_bit)) {
				unsigned int desired = 0;
				if(!__atomic_compare_exchange_n(&rw->__mlibc_rc,
						&rc_expected, desired, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
					continue;

				// Wake the futex.
				mlibc::sys_futex_wake((int *)&rw->__mlibc_rc);
				break;
			}else{
				unsigned int desired = (rc_expected & ~rc_count_mask) | (count - 1);
				if(!__atomic_compare_exchange_n(&rw->__mlibc_rc,
						&rc_expected, desired, false, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
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
