
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <frg/array.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>
#include <mlibc/thread.hpp>
#include <mlibc/tcb.hpp>

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

namespace {

unsigned int this_tid() {
	auto tcb = mlibc::get_current_tcb();
	return tcb->tid;
}

} // anonymous namespace

static constexpr unsigned int mutexRecursive = 1;
static constexpr unsigned int mutexErrorCheck = 2;

// TODO: either use uint32_t or determine the bit based on sizeof(int).
static constexpr unsigned int mutex_owner_mask = (static_cast<uint32_t>(1) << 30) - 1;
static constexpr unsigned int mutex_waiters_bit = static_cast<uint32_t>(1) << 31;

// Only valid for the internal __mlibc_m mutex of wrlocks.
static constexpr unsigned int mutex_excl_bit = static_cast<uint32_t>(1) << 30;

static constexpr unsigned int rc_count_mask = (static_cast<uint32_t>(1) << 31) - 1;
static constexpr unsigned int rc_waiters_bit = static_cast<uint32_t>(1) << 31;

// ----------------------------------------------------------------------------
// pthread_attr and pthread functions.
// ----------------------------------------------------------------------------

// pthread_attr functions.
int pthread_attr_init(pthread_attr_t *) {
	return 0;
}

int pthread_attr_destroy(pthread_attr_t *) {
	return 0;
}

int pthread_attr_getdetachstate(const pthread_attr_t *, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setdetachstate(pthread_attr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getstacksize(const pthread_attr_t *__restrict, size_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setstacksize(pthread_attr_t *, size_t) {
	mlibc::infoLogger() << "mlibc: pthread_attr_setstacksize() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setguardsize(pthread_attr_t *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getscope(const pthread_attr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setscope(pthread_attr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getschedpolicy(const pthread_attr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setschedpolicy(pthread_attr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getschedparam(const pthread_attr_t *__restrict, struct sched_param *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setschedparam(pthread_attr_t *__restrict, const struct sched_param *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getinheritsched(const pthread_attr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setinheritsched(pthread_attr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

extern "C" Tcb *__rtdl_allocateTcb();

// pthread functions.
int pthread_create(pthread_t *__restrict thread, const pthread_attr_t *__restrict,
		void *(*entry) (void *), void *__restrict user_arg) {
	auto new_tcb = __rtdl_allocateTcb();
	pid_t tid;
	mlibc::sys_clone(reinterpret_cast<void *>(entry), user_arg, new_tcb, &tid);
	*thread = reinterpret_cast<pthread_t>(new_tcb);

	__atomic_store_n(&new_tcb->tid, tid, __ATOMIC_RELAXED);
	mlibc::sys_futex_wake(&new_tcb->tid);

	return 0;
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

	struct key_local_info {
		void *value;
		uint64_t generation;
	};

	constinit frg::array<
		key_global_info,
		PTHREAD_KEYS_MAX
	> key_globals_{};

	thread_local constinit frg::array<
		key_local_info,
		PTHREAD_KEYS_MAX
	> key_locals_{};

	FutexLock key_mutex_;
}

int pthread_exit(void *ret_val) {
	auto self = mlibc::get_current_tcb();

	if (__atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED) & tcbExitingBit)
		return 0; // We are already exiting

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
			// FIXME: We need to lock here since we're accessing key_globals_, but
			// the dtor may call a function that also acquires the lock, resulting
			// in a deadlock.
			if (auto v = pthread_getspecific(i); v && key_globals_[i].dtor) {
				key_globals_[i].dtor(v);
				key_locals_[i].value = nullptr;
			}
		}
	}

	self->returnValue = ret_val;
	__atomic_store_n(&self->didExit, 1, __ATOMIC_RELEASE);
	mlibc::sys_futex_wake(&self->didExit);

	// TODO: clean up thread resources when we are detached.

	// TODO: do exit(0) when we're the only thread instead
	mlibc::sys_thread_exit();
	__builtin_unreachable();
}

int pthread_join(pthread_t thread, void **ret) {
	auto tcb = reinterpret_cast<Tcb *>(thread);

	if (!__atomic_load_n(&tcb->isJoinable, __ATOMIC_ACQUIRE))
		return EINVAL;

	while (!__atomic_load_n(&tcb->didExit, __ATOMIC_ACQUIRE)) {
		mlibc::sys_futex_wait(&tcb->didExit, 0, nullptr);
	}

	if (ret)
		*ret = tcb->returnValue;

	// FIXME: destroy tcb here, currently we leak it

	return 0;
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

int pthread_setname_np(pthread_t, const char *) {
	mlibc::infoLogger() << "mlibc: pthread_setname_np is a stub" << frg::endlog;
	return 0;
}

int pthread_getname_np(pthread_t, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_setstack(pthread_attr_t *, void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getstack(const pthread_attr_t *, void **, size_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_getattr_np(pthread_t, pthread_attr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_setschedparam(pthread_t, int, const struct sched_param *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_getschedparam(pthread_t, int *, struct sched_param *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
				tcb->returnValue = PTHREAD_CANCELED;

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
	auto tcb = reinterpret_cast<Tcb *>(thread);
	// Check if the TCB is valid, somewhat..
	if (tcb->selfPointer != tcb)
		return ESRCH;

	int old_value = tcb->cancelBits;
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
				if (!mlibc::sys_tgkill) {
					MLIBC_MISSING_SYSDEP();
					return ENOSYS;
				}

				return mlibc::sys_tgkill(pid, tcb->tid, SIGCANCEL);
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

	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return nullptr;

	if (key_globals_[key].generation > key_locals_[key].generation) {
		key_locals_[key].value = nullptr;
		key_locals_[key].generation = key_globals_[key].generation;
	}

	return key_locals_[key].value;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
	SCOPE_TRACE();

	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return EINVAL;

	key_locals_[key].value = const_cast<void *>(value);
	key_locals_[key].generation = key_globals_[key].generation;

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
			if(int e = mlibc::sys_futex_wait((int *)&once->__mlibc_done, onceLocked, nullptr); e)
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

	memset(attr, 0, sizeof(pthread_mutexattr_t));
	return 0;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t *) {
	SCOPE_TRACE();

	return 0;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type) {
	SCOPE_TRACE();

	// TODO: return EINVAL instead of asserting.
	__ensure(type == PTHREAD_MUTEX_NORMAL || type == PTHREAD_MUTEX_ERRORCHECK
			|| type == PTHREAD_MUTEX_RECURSIVE);
	attr->__mlibc_type = type;
	return 0;
}

int pthread_mutexattr_getrobust(const pthread_mutexattr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutexattr_setrobust(pthread_mutexattr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutexattr_getpshared(const pthread_mutexattr_t *, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutexattr_setpshared(pthread_mutexattr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// pthread_mutex functions
int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
		const pthread_mutexattr_t *__restrict attr) {
	SCOPE_TRACE();

	auto type = attr ? attr->__mlibc_type : 0;
	auto robust = attr ? attr->__mlibc_robust : 0;

	mutex->__mlibc_state = 0;
	mutex->__mlibc_recursion = 0;
	mutex->__mlibc_flags = 0;

	if(type == PTHREAD_MUTEX_RECURSIVE) {
		mutex->__mlibc_flags |= mutexRecursive;
	}else if(type == PTHREAD_MUTEX_ERRORCHECK) {
		mutex->__mlibc_flags |= mutexErrorCheck;
	}else{
		__ensure(type == PTHREAD_MUTEX_NORMAL);
	}

	__ensure(robust == PTHREAD_MUTEX_STALLED);

	return 0;
}
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	__ensure(!mutex->__mlibc_state);
	return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	unsigned int expected = 0;
	while(true) {
		if(!expected) {
			// Try to take the mutex here.
			if(__atomic_compare_exchange_n(&mutex->__mlibc_state,
					&expected, this_tid(), false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
				__ensure(!mutex->__mlibc_recursion);
				mutex->__mlibc_recursion = 1;
				return 0;
			}
		}else{
			// If this (recursive) mutex is already owned by us, increment the recursion level.
			if((expected & mutex_owner_mask) == this_tid()) {
				if(!(mutex->__mlibc_flags & mutexRecursive)) {
					if (mutex->__mlibc_flags & mutexErrorCheck)
						return EDEADLK;
					else
						mlibc::panicLogger() << "mlibc: pthread_mutex deadlock detected!"
							<< frg::endlog;
				}
				++mutex->__mlibc_recursion;
				return 0;
			}

			// Wait on the futex if the waiters flag is set.
			if(expected & mutex_waiters_bit) {
				if(int e = mlibc::sys_futex_wait((int *)&mutex->__mlibc_state, expected, nullptr); e)
					__ensure(!"sys_futex_wait() failed");

				// Opportunistically try to take the lock after we wake up.
				expected = 0;
			}else{
				// Otherwise we have to set the waiters flag first.
				unsigned int desired = expected | mutex_waiters_bit;
				if(__atomic_compare_exchange_n((int *)&mutex->__mlibc_state,
						reinterpret_cast<int*>(&expected), desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					expected = desired;
			}
		}
	}
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	unsigned int expected = __atomic_load_n(&mutex->__mlibc_state, __ATOMIC_RELAXED);
	if(!expected) {
		// Try to take the mutex here.
		if(__atomic_compare_exchange_n(&mutex->__mlibc_state,
						&expected, this_tid(), false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
			__ensure(!mutex->__mlibc_recursion);
			mutex->__mlibc_recursion = 1;
			return 0;
		}
	} else {
		// If this (recursive) mutex is already owned by us, increment the recursion level.
        if((expected & mutex_owner_mask) == this_tid()) {
            if(!(mutex->__mlibc_flags & mutexRecursive)) {
                if (mutex->__mlibc_flags & mutexErrorCheck)
                    return EDEADLK;
                else
                    mlibc::panicLogger() << "mlibc: pthread_mutex deadlock detected!"
                        << frg::endlog;
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

	// Decrement the recursion level and unlock if we hit zero.
	__ensure(mutex->__mlibc_recursion);
	if(--mutex->__mlibc_recursion)
		return 0;

	// Reset the mutex to the unlocked state.
	auto state = __atomic_exchange_n(&mutex->__mlibc_state, 0, __ATOMIC_RELEASE);

	if ((mutex->__mlibc_flags & mutexErrorCheck) && (state & mutex_owner_mask) != this_tid())
		return EPERM;

	if ((mutex->__mlibc_flags & mutexErrorCheck) && !(state & mutex_owner_mask))
		return EINVAL;

	__ensure((state & mutex_owner_mask) == this_tid());

	// Wake the futex if there were waiters.
	if(state & mutex_waiters_bit)
		if(int e = mlibc::sys_futex_wake((int *)&mutex->__mlibc_state); e)
			__ensure(!"sys_futex_wake() failed");
	return 0;
}

int pthread_mutex_consistent(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// pthread_condattr and pthread_cond functions.
// ----------------------------------------------------------------------------

int pthread_condattr_init(pthread_condattr_t *) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_condattr_init() is not implemented correctly" << frg::endlog;
	return 0;
}
int pthread_condattr_destroy(pthread_condattr_t *) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_condattr_destroy() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_condattr_getclock(const pthread_condattr_t *__restrict, clockid_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_condattr_setclock(pthread_condattr_t *, clockid_t) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_condattr_setclock() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_condattr_getpshared(const pthread_condattr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_condattr_setpshared(pthread_condattr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_cond_init(pthread_cond_t *__restrict cond, const pthread_condattr_t *__restrict) {
	SCOPE_TRACE();

	__atomic_store_n(&cond->__mlibc_seq, 1, __ATOMIC_RELAXED);

	return 0;
}

int pthread_cond_destroy(pthread_cond_t *) {
	SCOPE_TRACE();

	return 0;
}

int pthread_cond_wait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex) {
	return pthread_cond_timedwait(cond, mutex, nullptr);
}

int pthread_cond_timedwait(pthread_cond_t *__restrict cond, pthread_mutex_t *__restrict mutex,
		const struct timespec *__restrict abstime) {
	auto seq = __atomic_load_n(&cond->__mlibc_seq, __ATOMIC_ACQUIRE);
	// TODO: do proper error handling here.
	if(pthread_mutex_unlock(mutex))
		__ensure(!"Failed to unlock the mutex");
	if(mlibc::sys_futex_wait(&cond->__mlibc_seq, seq, abstime))
		__ensure(!"sys_futex_wait() failed");
	if(pthread_mutex_lock(mutex))
		__ensure(!"Failed to lock the mutex");
	return 0;
}

int pthread_cond_signal(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return pthread_cond_broadcast(cond);
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
	SCOPE_TRACE();

	__atomic_fetch_add(&cond->__mlibc_seq, 1, __ATOMIC_RELEASE);
	if(int e = mlibc::sys_futex_wake((int *)&cond->__mlibc_seq); e)
		__ensure(!"sys_futex_wake() failed");
	return 0;
}

// ----------------------------------------------------------------------------
// pthread_barrierattr and pthread_barrier functions.
// ----------------------------------------------------------------------------

int pthread_barrierattr_init(pthread_barrierattr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_barrierattr_destroy(pthread_barrierattr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_barrier_init(pthread_barrier_t *__restrict, const pthread_barrierattr_t *__restrict,
		unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_barrier_destroy(pthread_barrier_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_barrier_wait(pthread_barrier_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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

int pthread_rwlock_init(pthread_rwlock_t *__restrict rw, const pthread_rwlockattr_t *__restrict) {
	SCOPE_TRACE();
	rw->__mlibc_m = 0;
	rw->__mlibc_rc = 0;
	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *rw) {
	__ensure(!rw->__mlibc_m);
	__ensure(!rw->__mlibc_rc);
	return 0;
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

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

	// Increment the reader count while holding the __mlibc_m mutex.
	if(int e = rwlock_m_trylock(rw, false); e)
		return e;
	__atomic_fetch_add(&rw->__mlibc_rc, 1, __ATOMIC_ACQUIRE);
	rwlock_m_unlock(rw);

	return 0;
}

int pthread_rwlock_rdlock(pthread_rwlock_t *rw) {
	SCOPE_TRACE();

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
