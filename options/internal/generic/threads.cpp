#include <abi-bits/errno.h>
#include <bits/threads.h>
#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <frg/mutex.hpp>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/threads.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/time-helpers.hpp>

extern "C" Tcb *__rtld_allocateTcb();

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

} // namespace

extern "C" int __cxa_thread_atexit_impl(void (*function)(void *), void *argument,
		void *dso_symbol) {
	auto self = mlibc::get_current_tcb();
	auto handler = frg::construct<Tcb::CxaThreadExitHandler>(getAllocator());
	handler->function = function;
	handler->argument = argument;
	handler->dsoSymbol = dso_symbol;
	handler->next = self->cxaThreadExitHandlers;
	self->cxaThreadExitHandlers = handler;
	return 0;
}

extern "C" void __mlibc_do_cancel() {
	//TODO(geert): for now the same as pthread_exit()
	mlibc::thread_exit({__MLIBC_THREAD_CANCELED});
}

namespace mlibc {

void run_thread_local_destructors() {
	auto self = get_current_tcb();
	while (self->cxaThreadExitHandlers) {
		auto handler = self->cxaThreadExitHandlers;
		self->cxaThreadExitHandlers = handler->next;
		handler->function(handler->argument);
		frg::destruct(getAllocator(), handler);
	}
}

int thread_once(__mlibc_once *once, void (*func) (void)) {
	auto expected = once->__mlibc_done.load(std::memory_order_acquire);

	// fast path: the function was already run.
	while(!(expected & __mlibc_once::onceComplete)) {
		if(!expected) {
			// try to acquire the mutex.
			if (!once->__mlibc_done.compare_exchange_weak(
			        expected, __mlibc_once::onceLocked, std::memory_order_acquire, std::memory_order_acquire
			    ))
				continue;

			func();

			// unlock the mutex.
			once->__mlibc_done.exchange(__mlibc_once::onceComplete, std::memory_order_release);
			if(int e = mlibc::sysdep<FutexWake>((int *)&once->__mlibc_done, true); e)
				__ensure(!"sys_futex_wake() failed");
			return 0;
		}else{
			// a different thread is currently running the initializer.
			__ensure(expected == __mlibc_once::onceLocked);
			// if the wait gets interrupted by a signal, check again.
			// EAGAIN will also be a retry, as it means the other thread completed
			// and changed the __mlibc_done variable to signal it before we actually went to sleep.
			if(int e = sysdep<FutexWait>((int *)&once->__mlibc_done, __mlibc_once::onceLocked, nullptr); e && e != EINTR && e != EAGAIN)
				__ensure(!"sys_futex_wait() failed");
			expected = once->__mlibc_done.load(std::memory_order_acquire);
		}
	}

	return 0;
}

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int) {
	auto new_tcb = __rtld_allocateTcb();
	pid_t tid;
	struct __mlibc_threadattr attr = {};
	if (attrp)
		attr = *attrp;

	if (attr.__cpuset)
		mlibc::infoLogger() << "pthread_create(): cpuset is ignored!" << frg::endlog;
	if (attr.__sigmaskset)
		mlibc::infoLogger() << "pthread_create(): sigmask is ignored!" << frg::endlog;

	// TODO: due to alignment guarantees, the stackaddr and stacksize might change
	// when the stack is allocated. Currently this isn't propagated to the TCB,
	// but it should be.
	void *stack = attr.__stackaddr;
	if (!IsImplemented<PrepareStack>) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	int ret = sysdep_or_panic<PrepareStack>(&stack, entry,
			user_arg, new_tcb, &attr.__stacksize, &attr.__guardsize, &new_tcb->stackAddr);
	if (ret)
		return ret;

	if (!IsImplemented<Clone>) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	new_tcb->stackSize = attr.__stacksize;
	new_tcb->guardSize = attr.__guardsize;
	new_tcb->returnValueType = (returns_int) ? TcbThreadReturnValue::Integer : TcbThreadReturnValue::Pointer;
	new_tcb->isJoinable = (attr.__detachstate == __MLIBC_THREAD_CREATE_JOINABLE);
	__atomic_store_n(&new_tcb->cancelBits, 0, __ATOMIC_RELAXED);
	sysdep_or_panic<Clone>(new_tcb, &tid, stack);
	*thread = reinterpret_cast<struct __mlibc_thread_data *>(new_tcb);

	__atomic_store_n(&new_tcb->tid, tid, __ATOMIC_RELAXED);
	mlibc::sysdep<FutexWake>(&new_tcb->tid, true);

	return 0;
}

int thread_join(struct __mlibc_thread_data *thread, void *ret) {
	auto tcb = reinterpret_cast<Tcb *>(thread);

	if(!tcb->isJoinable) {
		mlibc::infoLogger() << "mlibc: pthread_join() called on a detached thread" << frg::endlog;
		return EINVAL;
	}

	if (!__atomic_load_n(&tcb->isJoinable, __ATOMIC_ACQUIRE))
		return EINVAL;

	mlibc::thread_testcancel();

	while (!__atomic_load_n(&tcb->didExit, __ATOMIC_ACQUIRE)) {
		if (int e = sysdep<FutexWait>(&tcb->didExit, 0, nullptr); e == EINTR)
			mlibc::thread_testcancel();
	}

	if(ret && tcb->returnValueType == TcbThreadReturnValue::Pointer)
		*reinterpret_cast<void **>(ret) = tcb->returnValue.voidPtr;
	else if(ret && tcb->returnValueType == TcbThreadReturnValue::Integer)
		*reinterpret_cast<int *>(ret) = tcb->returnValue.integer;

	// FIXME: destroy tcb here, currently we leak it

	return 0;
}

int thread_detach(struct __mlibc_thread_data *thread) {
	auto tcb = reinterpret_cast<Tcb *>(thread);
	if (!__atomic_load_n(&tcb->isJoinable, __ATOMIC_RELAXED))
		return EINVAL;

	int expected = 1;
	if(!__atomic_compare_exchange_n(&tcb->isJoinable, &expected, 0, false, __ATOMIC_RELEASE,
				__ATOMIC_RELAXED))
		return EINVAL;

	return 0;
}

namespace {

__attribute__ ((__noreturn__)) void do_exit() {
	mlibc::sysdep_or_panic<ThreadExit>();
	__builtin_unreachable();
}

} // namespace

__attribute__ ((__noreturn__)) void thread_exit(thread_exit_return ret_val) {
	auto self = get_current_tcb();

	if (__atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED) & tcbExitingBit)
		mlibc::do_exit();

	__atomic_fetch_or(&self->cancelBits, tcbExitingBit, __ATOMIC_RELAXED);

	run_thread_local_destructors();

	while (!self->cleanupHandlers.empty()) {
		auto hand = self->cleanupHandlers.pop_back();
		hand->func(hand->arg);
		frg::destruct(getAllocator(), hand);
	}

	for (size_t j = 0; j < __MLIBC_THREAD_DESTRUCTOR_ITERATIONS; j++) {
		for (size_t i = 0; i < PTHREAD_KEYS_MAX; i++) {
			if (auto v = thread_key_get(i)) {
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

	self->returnValue = ret_val;

	__atomic_store_n(&self->didExit, 1, __ATOMIC_RELEASE);
	sysdep<FutexWake>(&self->didExit, true);

	// TODO: clean up thread resources when we are detached.

	// TODO: do exit(0) when we're the only thread instead
	mlibc::do_exit();
}

int thread_mutex_init(struct __mlibc_mutex *__restrict mutex,
		const struct __mlibc_mutexattr *__restrict attr) {
	auto type = attr ? attr->__mlibc_type : __MLIBC_THREAD_MUTEX_DEFAULT;
	auto robust = attr ? attr->__mlibc_robust : __MLIBC_THREAD_MUTEX_STALLED;
	auto protocol = attr ? attr->__mlibc_protocol : __MLIBC_THREAD_PRIO_NONE;
	auto pshared = attr ? attr->__mlibc_pshared : __MLIBC_THREAD_PROCESS_PRIVATE;

	new (mutex) __mlibc_mutex();

	if(type == __MLIBC_THREAD_MUTEX_RECURSIVE) {
		mutex->__mlibc_flags |= __mlibc_mutex::mutexRecursive;
	}else if(type == __MLIBC_THREAD_MUTEX_ERRORCHECK) {
		mutex->__mlibc_flags |= __mlibc_mutex::mutexErrorCheck;
	}else{
		__ensure(type == __MLIBC_THREAD_MUTEX_NORMAL);
	}

	if (pshared == __MLIBC_THREAD_PROCESS_SHARED)
		mutex->__mlibc_flags |= __mlibc_mutex::mutexShared;

	// TODO: Other values aren't supported yet.
	__ensure(robust == __MLIBC_THREAD_MUTEX_STALLED);

	if(protocol != __MLIBC_THREAD_PRIO_NONE)
		infoLogger() << "mlibc: mutex priority " << protocol << " is not implemented" << frg::endlog;

	return 0;
}

int thread_mutex_destroy(struct __mlibc_mutex *mutex) {
	__ensure(!mutex->__mlibc_state);
	return 0;
}

int thread_mutex_timedlock(struct __mlibc_mutex *mutex, const struct timespec *__restrict abstime, clockid_t clockid) {
	unsigned int this_tid = mlibc::this_tid();
	unsigned int expected = 0;
	while(true) {
		if(!expected) {
			// Try to take the mutex here.
			if(__atomic_compare_exchange_n(&mutex->__mlibc_state,
					&expected, this_tid | __mlibc_mutex::mutex_waiters_bit, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
				__ensure(!mutex->__mlibc_recursion);
				mutex->__mlibc_recursion = 1;
				return 0;
			}
		}else{
			// If this (recursive) mutex is already owned by us, increment the recursion level.
			if((expected & __mlibc_mutex::mutex_owner_mask) == this_tid) {
				if(!(mutex->__mlibc_flags & __mlibc_mutex::mutexRecursive)) {
					return EDEADLK;
				} else {
					++mutex->__mlibc_recursion;
					return 0;
				}
			}

			// Wait on the futex if the waiters flag is set.
			if(expected & __mlibc_mutex::mutex_waiters_bit) {
				int e;
				if (abstime) {
					// Adjust for the fact that sys_futex_wait accepts a *timeout*, but
					// we accept an *absolute time*.
					struct timespec timeout;
					if (!mlibc::time_absolute_to_relative(clockid, abstime, &timeout))
						return EINVAL;

					if (timeout.tv_sec == 0 && timeout.tv_nsec == 0)
						return ETIMEDOUT;

					e = sysdep<FutexWait>((int *)&mutex->__mlibc_state, expected, &timeout);

					if (e == ETIMEDOUT)
						return e;
				} else {
					e = sysdep<FutexWait>((int *)&mutex->__mlibc_state, expected, nullptr);
				}

				// If the wait returns EAGAIN, that means that the mutex_waiters_bit was just unset by
				// some other thread. In this case, we should loop back around.
				// Also do so in case of a signal being caught.
				if (e && e != EAGAIN && e != EINTR)
					mlibc::panicLogger() << "sys_futex_wait() failed with error code " << e << frg::endlog;

				// Opportunistically try to take the lock after we wake up.
				expected = 0;
			}else{
				// Otherwise we have to set the waiters flag first.
				unsigned int desired = expected | __mlibc_mutex::mutex_waiters_bit;
				if(__atomic_compare_exchange_n((int *)&mutex->__mlibc_state,
						reinterpret_cast<int*>(&expected), desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					expected = desired;
			}
		}
	}
}

int thread_mutex_lock(struct __mlibc_mutex *mutex) {
	return thread_mutex_timedlock(mutex, nullptr, 0);
}

int thread_mutex_trylock(struct __mlibc_mutex *mutex) {
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
		if((expected & __mlibc_mutex::mutex_owner_mask) == this_tid) {
			if(!(mutex->__mlibc_flags & __mlibc_mutex::mutexRecursive)) {
				return EBUSY;
			}
			++mutex->__mlibc_recursion;
			return 0;
		}
	}

	return EBUSY;
}

int thread_mutex_unlock(struct __mlibc_mutex *mutex) {
	// Decrement the recursion level and unlock if we hit zero.
	__ensure(mutex->__mlibc_recursion);
	if(--mutex->__mlibc_recursion)
		return 0;

	auto flags = mutex->__mlibc_flags;

	// Reset the mutex to the unlocked state.
	auto state = __atomic_exchange_n(&mutex->__mlibc_state, 0, __ATOMIC_RELEASE);

	// After this point the mutex is unlocked, and therefore we cannot access its contents as it
	// may have been destroyed by another thread.

	unsigned int this_tid = mlibc::this_tid();
	if ((flags & __mlibc_mutex::mutexErrorCheck) && (state & __mlibc_mutex::mutex_owner_mask) != this_tid)
		return EPERM;

	if ((flags & __mlibc_mutex::mutexErrorCheck) && !(state & __mlibc_mutex::mutex_owner_mask))
		return EINVAL;

	__ensure((state & __mlibc_mutex::mutex_owner_mask) == this_tid);

	if(state & __mlibc_mutex::mutex_waiters_bit) {
		// Wake the futex if there were waiters. Since the mutex might not exist at this location
		// anymore, we must conservatively ignore EACCES and EINVAL which may occur as a result.
		int e = mlibc::sysdep<FutexWake>((int *)&mutex->__mlibc_state, true);
		__ensure(e >= 0 || e == EACCES || e == EINVAL);
	}

	return 0;
}

int thread_mutexattr_gettype(const struct __mlibc_mutexattr *__restrict attr, int *__restrict type) {
	*type = attr->__mlibc_type;
	return 0;
}

int thread_mutexattr_settype(struct __mlibc_mutexattr *attr, int type) {
	if (type != __MLIBC_THREAD_MUTEX_NORMAL && type != __MLIBC_THREAD_MUTEX_ERRORCHECK
			&& type != __MLIBC_THREAD_MUTEX_RECURSIVE)
		return EINVAL;

	attr->__mlibc_type = type;
	return 0;
}

int thread_cond_init(struct __mlibc_cond *__restrict cond, const struct __mlibc_condattr *__restrict attr) {
	new (cond) __mlibc_cond();

	if (attr) {
		cond->__mlibc_clock = attr->__mlibc_clock;
		cond->__mlibc_flags = attr->__mlibc_pshared;
	}

	return 0;
}

int thread_cond_destroy(struct __mlibc_cond *) {
	return 0;
}

int thread_cond_signal(struct __mlibc_cond *cond) {
	cond->__mlibc_seq.fetch_add(1, std::memory_order_release);
	if(int e = mlibc::sysdep<FutexWake>((int *)&cond->__mlibc_seq, false); e)
		__ensure(!"sys_futex_wake() failed");

	return 0;
}

int thread_cond_broadcast(struct __mlibc_cond *cond) {
	cond->__mlibc_seq.fetch_add(1, std::memory_order_release);
	if(int e = mlibc::sysdep<FutexWake>((int *)&cond->__mlibc_seq, true); e)
		__ensure(!"sys_futex_wake() failed");

	return 0;
}

int thread_cond_timedwait(struct __mlibc_cond *__restrict cond, __mlibc_mutex *__restrict mutex,
		const struct timespec *__restrict abstime, clockid_t clockid) {
	mlibc::thread_testcancel();

	constexpr long nanos_per_second = 1'000'000'000;
	if (abstime && (abstime->tv_nsec < 0 || abstime->tv_nsec >= nanos_per_second))
		return EINVAL;

	auto seq = cond->__mlibc_seq.load(std::memory_order_acquire);

	// TODO: handle locking errors and cancellation properly.
	while (true) {
		if (thread_mutex_unlock(mutex))
			__ensure(!"Failed to unlock the mutex");

		int e;
		if (abstime) {
			// Adjust for the fact that sys_futex_wait accepts a *timeout*, but
			// pthread_cond_timedwait accepts an *absolute time*.
			struct timespec timeout;
			if (!mlibc::time_absolute_to_relative(clockid, abstime, &timeout)) {
				if (thread_mutex_lock(mutex))
					__ensure(!"Failed to lock the mutex");
				return EINVAL;
			} else if (timeout.tv_sec == 0 && timeout.tv_nsec == 0) {
				if (thread_mutex_lock(mutex))
					__ensure(!"Failed to lock the mutex");
				return ETIMEDOUT;
			}

			e = sysdep<FutexWait>((int *)&cond->__mlibc_seq, seq, &timeout);
		} else {
			e = sysdep<FutexWait>((int *)&cond->__mlibc_seq, seq, nullptr);
		}

		if (thread_mutex_lock(mutex))
			__ensure(!"Failed to lock the mutex");

		// There are four cases to handle:
		//   1. e == 0: this indicates a (potentially spurious) wakeup. The value of
		//      seq *must* be checked to distinguish these two cases.
		//   2. e == EAGAIN: this indicates that the value of seq changed before we
		//      went to sleep. We don't need to check seq in this case.
		//   3. e == EINTR: a signal was delivered. The man page allows us to choose
		//      whether to go to sleep again or to return 0, but we do the former
		//      to match other libcs.
		//   4. e == ETIMEDOUT: this should only happen if abstime is set.
		if (e == 0) {
			auto cur_seq = cond->__mlibc_seq.load(std::memory_order_acquire);
			if (cur_seq > seq)
				return 0;
		} else if (e == EAGAIN) {
			__ensure(cond->__mlibc_seq.load(std::memory_order_acquire) > seq);
			return 0;
		} else if (e == EINTR) {
			mlibc::thread_testcancel();
			continue;
		} else if (e == ETIMEDOUT) {
			__ensure(abstime);
			return ETIMEDOUT;
		} else {
			mlibc::panicLogger() << "sys_futex_wait() failed with error " << e << frg::endlog;
		}
	}
}

int thread_key_create(__mlibc_uintptr *out, void (*destructor)(void *)) {
	auto g = frg::guard(&key_mutex_);

	__mlibc_uintptr key = PTHREAD_KEYS_MAX;
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

int thread_key_delete(__mlibc_uintptr key) {
	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return EINVAL;

	key_globals_[key].in_use = false;
	key_globals_[key].dtor = nullptr;
	key_globals_[key].generation++;

	return 0;
}

void *thread_key_get(__mlibc_uintptr key) {
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

int thread_key_set(__mlibc_uintptr key, const void *value) {
	auto self = mlibc::get_current_tcb();
	auto g = frg::guard(&key_mutex_);

	if (key >= PTHREAD_KEYS_MAX || !key_globals_[key].in_use)
		return EINVAL;

	(*self->localKeys)[key].value = const_cast<void *>(value);
	(*self->localKeys)[key].generation = key_globals_[key].generation;

	return 0;
}

void thread_testcancel(void) {
	auto self = mlibc::get_current_tcb();
	int value = __atomic_load_n(&self->cancelBits, __ATOMIC_RELAXED);
	if ((value & tcbCancelEnableBit) && (value & tcbCancelTriggerBit)) {
		__mlibc_do_cancel();
		__builtin_unreachable();
	}
}

} // namespace mlibc
