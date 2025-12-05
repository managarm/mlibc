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

namespace mlibc {

static constexpr unsigned int onceComplete = 1;
static constexpr unsigned int onceLocked = 2;

int thread_once(__mlibc_once *once, void (*func) (void)) {
	auto expected = __atomic_load_n(&once->__mlibc_done, __ATOMIC_ACQUIRE);

	// fast path: the function was already run.
	while(!(expected & onceComplete)) {
		if(!expected) {
			// try to acquire the mutex.
			if(!__atomic_compare_exchange_n(&once->__mlibc_done,
					&expected, onceLocked, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
				continue;

			func();

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

int thread_create(struct __mlibc_thread_data **__restrict thread, const struct __mlibc_threadattr *__restrict attrp, void *entry, void *__restrict user_arg, bool returns_int) {
	auto new_tcb = __rtld_allocateTcb();
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
			user_arg, new_tcb, &attr.__mlibc_stacksize, &attr.__mlibc_guardsize, &new_tcb->stackAddr);
	if (ret)
		return ret;

	if (!mlibc::sys_clone) {
		MLIBC_MISSING_SYSDEP();
		return ENOSYS;
	}
	new_tcb->stackSize = attr.__mlibc_stacksize;
	new_tcb->guardSize = attr.__mlibc_guardsize;
	new_tcb->returnValueType = (returns_int) ? TcbThreadReturnValue::Integer : TcbThreadReturnValue::Pointer;
	new_tcb->isJoinable = (attr.__mlibc_detachstate == __MLIBC_THREAD_CREATE_JOINABLE);
	mlibc::sys_clone(new_tcb, &tid, stack);
	*thread = reinterpret_cast<struct __mlibc_thread_data *>(new_tcb);

	__atomic_store_n(&new_tcb->tid, tid, __ATOMIC_RELAXED);
	mlibc::sys_futex_wake(&new_tcb->tid);

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

	while (!__atomic_load_n(&tcb->didExit, __ATOMIC_ACQUIRE)) {
		mlibc::sys_futex_wait(&tcb->didExit, 0, nullptr);
	}

	if(ret && tcb->returnValueType == TcbThreadReturnValue::Pointer)
		*reinterpret_cast<void **>(ret) = tcb->returnValue.voidPtr;
	else if(ret && tcb->returnValueType == TcbThreadReturnValue::Integer)
		*reinterpret_cast<int *>(ret) = tcb->returnValue.intVal;

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
	sys_thread_exit();
	__builtin_unreachable();
}

} // namespace

__attribute__ ((__noreturn__)) void thread_exit(thread_exit_return ret_val) {
	auto self = get_current_tcb();

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

	if(self->returnValueType == TcbThreadReturnValue::Pointer)
		self->returnValue.voidPtr = ret_val.voidPtr;
	else if(self->returnValueType == TcbThreadReturnValue::Integer)
		self->returnValue.intVal = ret_val.integer;

	__atomic_store_n(&self->didExit, 1, __ATOMIC_RELEASE);
	sys_futex_wake(&self->didExit);

	// TODO: clean up thread resources when we are detached.

	// TODO: do exit(0) when we're the only thread instead
	mlibc::do_exit();
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

static constexpr unsigned int mutexRecursive = 1;
static constexpr unsigned int mutexErrorCheck = 2;

// TODO: either use uint32_t or determine the bit based on sizeof(int).
static constexpr unsigned int mutex_owner_mask = (static_cast<uint32_t>(1) << 30) - 1;
static constexpr unsigned int mutex_waiters_bit = static_cast<uint32_t>(1) << 31;

int thread_mutex_init(struct __mlibc_mutex *__restrict mutex,
		const struct __mlibc_mutexattr *__restrict attr) {
	auto type = attr ? attr->__mlibc_type : __MLIBC_THREAD_MUTEX_DEFAULT;
	auto robust = attr ? attr->__mlibc_robust : __MLIBC_THREAD_MUTEX_STALLED;
	auto protocol = attr ? attr->__mlibc_protocol : __MLIBC_THREAD_PRIO_NONE;
	auto pshared = attr ? attr->__mlibc_pshared : __MLIBC_THREAD_PROCESS_PRIVATE;

	mutex->__mlibc_state = 0;
	mutex->__mlibc_recursion = 0;
	mutex->__mlibc_flags = 0;
	mutex->__mlibc_prioceiling = 0; // TODO: We don't implement this.

	if(type == __MLIBC_THREAD_MUTEX_RECURSIVE) {
		mutex->__mlibc_flags |= mutexRecursive;
	}else if(type == __MLIBC_THREAD_MUTEX_ERRORCHECK) {
		mutex->__mlibc_flags |= mutexErrorCheck;
	}else{
		__ensure(type == __MLIBC_THREAD_MUTEX_NORMAL);
	}

	// TODO: Other values aren't supported yet.
	__ensure(robust == __MLIBC_THREAD_MUTEX_STALLED);
	__ensure(protocol == __MLIBC_THREAD_PRIO_NONE);
	__ensure(pshared == __MLIBC_THREAD_PROCESS_PRIVATE);

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
					&expected, this_tid, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)) {
				__ensure(!mutex->__mlibc_recursion);
				mutex->__mlibc_recursion = 1;
				return 0;
			}
		}else{
			// If this (recursive) mutex is already owned by us, increment the recursion level.
			if((expected & mutex_owner_mask) == this_tid) {
				if(!(mutex->__mlibc_flags & mutexRecursive)) {
					if (!abstime)
						return EDEADLK;
				} else {
					++mutex->__mlibc_recursion;
					return 0;
				}
			}

			// Wait on the futex if the waiters flag is set.
			if(expected & mutex_waiters_bit) {
				int e;
				if (abstime) {
					// Adjust for the fact that sys_futex_wait accepts a *timeout*, but
					// we accept an *absolute time*.
					struct timespec timeout;
					if (!mlibc::time_absolute_to_relative(clockid, abstime, &timeout))
						return EINVAL;

					if (timeout.tv_sec == 0 && timeout.tv_nsec == 0)
						return ETIMEDOUT;

					e = mlibc::sys_futex_wait((int *)&mutex->__mlibc_state, expected, &timeout);

					if (e == ETIMEDOUT)
						return e;
				} else {
					e = mlibc::sys_futex_wait((int *)&mutex->__mlibc_state, expected, nullptr);
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
				unsigned int desired = expected | mutex_waiters_bit;
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
	if ((flags & mutexErrorCheck) && (state & mutex_owner_mask) != this_tid)
		return EPERM;

	if ((flags & mutexErrorCheck) && !(state & mutex_owner_mask))
		return EINVAL;

	__ensure((state & mutex_owner_mask) == this_tid);

	if(state & mutex_waiters_bit) {
		// Wake the futex if there were waiters. Since the mutex might not exist at this location
		// anymore, we must conservatively ignore EACCES and EINVAL which may occur as a result.
		int e = mlibc::sys_futex_wake((int *)&mutex->__mlibc_state);
		__ensure(e >= 0 || e == EACCES || e == EINVAL);
	}

	return 0;
}

int thread_mutexattr_init(struct __mlibc_mutexattr *attr) {
	attr->__mlibc_type = __MLIBC_THREAD_MUTEX_DEFAULT;
	attr->__mlibc_robust = __MLIBC_THREAD_MUTEX_STALLED;
	attr->__mlibc_pshared = __MLIBC_THREAD_PROCESS_PRIVATE;
	attr->__mlibc_protocol = __MLIBC_THREAD_PRIO_NONE;
	return 0;
}

int thread_mutexattr_destroy(struct __mlibc_mutexattr *attr) {
	memset(attr, 0, sizeof(*attr));
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
	auto clock = attr ? attr->__mlibc_clock : CLOCK_REALTIME;
	auto pshared = attr ? attr->__mlibc_pshared : __MLIBC_THREAD_PROCESS_PRIVATE;

	cond->__mlibc_clock = clock;
	cond->__mlibc_flags = pshared;

	__atomic_store_n(&cond->__mlibc_seq, 1, __ATOMIC_RELAXED);

	return 0;
}

int thread_cond_destroy(struct __mlibc_cond *) {
	return 0;
}

int thread_cond_broadcast(struct __mlibc_cond *cond) {
	__atomic_fetch_add(&cond->__mlibc_seq, 1, __ATOMIC_RELEASE);
	if(int e = mlibc::sys_futex_wake((int *)&cond->__mlibc_seq); e)
		__ensure(!"sys_futex_wake() failed");

	return 0;
}

int thread_cond_timedwait(struct __mlibc_cond *__restrict cond, __mlibc_mutex *__restrict mutex,
		const struct timespec *__restrict abstime, clockid_t clockid) {
	// TODO: pshared isn't supported yet.
	__ensure(cond->__mlibc_flags == 0);

	constexpr long nanos_per_second = 1'000'000'000;
	if (abstime && (abstime->tv_nsec < 0 || abstime->tv_nsec >= nanos_per_second))
		return EINVAL;

	auto seq = __atomic_load_n(&cond->__mlibc_seq, __ATOMIC_ACQUIRE);

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

			e = mlibc::sys_futex_wait((int *)&cond->__mlibc_seq, seq, &timeout);
		} else {
			e = mlibc::sys_futex_wait((int *)&cond->__mlibc_seq, seq, nullptr);
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
			auto cur_seq = __atomic_load_n(&cond->__mlibc_seq, __ATOMIC_ACQUIRE);
			if (cur_seq > seq)
				return 0;
		} else if (e == EAGAIN) {
			__ensure(__atomic_load_n(&cond->__mlibc_seq, __ATOMIC_ACQUIRE) > seq);
			return 0;
		} else if (e == EINTR) {
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

} // namespace mlibc
