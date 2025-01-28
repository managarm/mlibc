#include <abi-bits/errno.h>
#include <bits/threads.h>
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/lock.hpp>
#include <mlibc/threads.hpp>
#include <mlibc/tcb.hpp>

extern "C" Tcb *__rtld_allocateTcb();

namespace mlibc {

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
	mlibc::sys_clone(new_tcb, &tid, stack);
	*thread = reinterpret_cast<struct __mlibc_thread_data *>(new_tcb);

	__atomic_store_n(&new_tcb->tid, tid, __ATOMIC_RELAXED);
	mlibc::sys_futex_wake(&new_tcb->tid);

	return 0;
}

int thread_join(struct __mlibc_thread_data *thread, void *ret) {
	auto tcb = reinterpret_cast<Tcb *>(thread);

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

int thread_mutex_lock(struct __mlibc_mutex *mutex) {
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
				int e = mlibc::sys_futex_wait((int *)&mutex->__mlibc_state, expected, nullptr);

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
		const struct timespec *__restrict abstime) {
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
			// Note: mlibc::sys_clock_get is available unconditionally.
			struct timespec now;
			if (mlibc::sys_clock_get(cond->__mlibc_clock, &now.tv_sec, &now.tv_nsec))
				__ensure(!"sys_clock_get() failed");

			struct timespec timeout;
			timeout.tv_sec = abstime->tv_sec - now.tv_sec;
			timeout.tv_nsec = abstime->tv_nsec - now.tv_nsec;

			// Check if abstime has already passed.
			if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_nsec < 0)) {
				if (thread_mutex_lock(mutex))
					__ensure(!"Failed to lock the mutex");
				return ETIMEDOUT;
			} else if (timeout.tv_nsec >= nanos_per_second) {
				timeout.tv_nsec -= nanos_per_second;
				timeout.tv_sec++;
				__ensure(timeout.tv_nsec < nanos_per_second);
			} else if (timeout.tv_nsec < 0) {
				timeout.tv_nsec += nanos_per_second;
				timeout.tv_sec--;
				__ensure(timeout.tv_nsec >= 0);
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

} // namespace mlibc
