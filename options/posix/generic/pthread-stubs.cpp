
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

#include <bits/ensure.h>
#include <frg/allocation.hpp>
#include <frg/hash_map.hpp>
#include <mlibc/allocator.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>
#include <mlibc/thread.hpp>

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
	if(mlibc::sys_futex_tid)
		return mlibc::sys_futex_tid();
	return 1;
}

} // anonymous namespace

static constexpr unsigned int mutexRecursive = 1;
static constexpr unsigned int mutexErrorCheck = 2;

// TODO: either use uint32_t or determine the bit based on sizeof(int).
static constexpr unsigned int mutex_owner_mask = (static_cast<uint32_t>(1) << 31) - 1;
static constexpr unsigned int mutex_waiters_bit = static_cast<uint32_t>(1) << 31;

// ----------------------------------------------------------------------------
// pthread_attr and pthread functions.
// ----------------------------------------------------------------------------

// pthread_attr functions.
int pthread_attr_init(pthread_attr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_attr_destroy(pthread_attr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_attr_setguardsize(pthread_attr_t *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

extern "C" void *__rtdl_allocateTcb();

// pthread functions.
int pthread_create(pthread_t *__restrict thread, const pthread_attr_t *__restrict,
		void *(*entry) (void *), void *__restrict user_arg) {

	void *new_tcb = __rtdl_allocateTcb();
	mlibc::sys_clone(reinterpret_cast<void *>(entry), user_arg, new_tcb, nullptr);
	*thread = reinterpret_cast<pthread_t>(new_tcb);

	return 0;
}

pthread_t pthread_self(void) {
	return reinterpret_cast<pthread_t>(mlibc::get_current_tcb());
}

int pthread_equal(pthread_t, pthread_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_exit(void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_join(pthread_t, void **) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_detach(pthread_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_cleanup_push(void (*) (void *), void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cleanup_pop(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_setcanceltype(int, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_setcancelstate(int, int *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void pthread_testcancel(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cancel(pthread_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_atfork(void (*) (void), void (*) (void), void (*) (void)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// pthread_key functions.
// ----------------------------------------------------------------------------

struct __mlibc_key_data {
	__mlibc_key_data()
	: mutex(PTHREAD_MUTEX_INITIALIZER),
			values{frg::hash<int>{}, getAllocator()} { }

	pthread_mutex_t mutex;

	// TODO: this should be unsigned int.
	frg::hash_map<int, void *, frg::hash<int>, MemoryAllocator> values;
};

int pthread_key_create(pthread_key_t *out_key, void (*destructor) (void *)) {
	SCOPE_TRACE();
	// TODO: Invoke the destructor on thread exit.
	*out_key = frg::construct<__mlibc_key_data>(getAllocator());
	return 0;
}

int pthread_key_delete(pthread_key_t key) {
	SCOPE_TRACE();
	frg::destruct(getAllocator(), key);
	return 0;
}

void *pthread_getspecific(pthread_key_t key) {
	SCOPE_TRACE();

	if(pthread_mutex_lock(&key->mutex))
		__ensure("Could not lock mutex");
	
	void *value = nullptr;
	auto it = key->values.get(this_tid());
	if(it)
		value = *it;

	if(pthread_mutex_unlock(&key->mutex))
		__ensure("Could not unlock mutex");
	
	return value;
}

int pthread_setspecific(pthread_key_t key, const void *value) {
	SCOPE_TRACE();

	if(pthread_mutex_lock(&key->mutex))
		__ensure("Could not lock mutex");
	
	auto it = key->values.get(this_tid());
	if(it) {
		*it = const_cast<void *>(value);
	}else{
		key->values.insert(this_tid(), const_cast<void *>(value));
	}

	if(pthread_mutex_unlock(&key->mutex))
		__ensure("Could not unlock mutex");
	
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
			if(int e = mlibc::sys_futex_wait((int *)&once->__mlibc_done, onceLocked); e)
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
				if(int e = mlibc::sys_futex_wait((int *)&mutex->__mlibc_state, expected); e)
					__ensure(!"sys_futex_wait() failed");
				
				// Opportunistically try to take the lock after we wake up.
				expected = 0;
			}else{
				// Otherwise we have to set the waiters flag first.
				unsigned int desired = expected | mutex_waiters_bit;
				if(__atomic_compare_exchange_n((int *)&mutex->__mlibc_state,
						&expected, desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					expected = desired;
			}
		}
	}
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_mutex_trylock() is not implemented correctly" << frg::endlog;
	if(pthread_mutex_lock(mutex)) {
		return EBUSY;
	}
	return 0;
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
/*
	auto seq = __atomic_load_n(&cond->__mlibc_seq, __ATOMIC_RELAXED);
	// TODO: do proper error handling here.
	if(pthread_mutex_unlock(mutex))
		__ensure(!"Failed to unlock the mutex");
	if(mlibc::sys_futex_wait(&cond->__mlibc_seq, seq))
		__ensure(!"sys_futex_wait() failed");
	return 0;*/
}
int pthread_cond_timedwait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict,
		const struct timespec *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_cond_signal(pthread_cond_t *cond) {
	SCOPE_TRACE();

	return pthread_cond_broadcast(cond);
}

int pthread_cond_broadcast(pthread_cond_t *cond) {
	SCOPE_TRACE();

	__atomic_fetch_add(&cond->__mlibc_seq, 1, __ATOMIC_RELAXED);
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

int pthread_rwlock_init(pthread_rwlock_t *__restrict, const pthread_rwlockattr_t *__restrict) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_rwlock_init() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_rwlock_destroy(pthread_rwlock_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_rwlock_trywrlock(pthread_rwlock_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_rwlock_wrlock(pthread_rwlock_t *) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_rwlock_wrlock() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_rwlock_tryrdlock(pthread_rwlock_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_rwlock_rdlock(pthread_rwlock_t *) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_rwlock_rdlock() is not implemented correctly" << frg::endlog;
	return 0;
}

int pthread_rwlock_unlock(pthread_rwlock_t *) {
	SCOPE_TRACE();

	mlibc::infoLogger() << "mlibc: pthread_rwlock_unlock() is not implemented correctly" << frg::endlog;
	return 0;
}
