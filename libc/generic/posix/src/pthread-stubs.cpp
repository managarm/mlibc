
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <frigg/traits.hpp>
#include <frigg/memory.hpp>
#include <frigg/hashmap.hpp>
#include <hel.h>
#include <hel-syscalls.h>

#include <mlibc/ensure.h>
#include <mlibc/frigg-alloc.hpp>

// TODO: move this to a 

static bool enableTrace = false;

struct ScopeTrace {
	ScopeTrace(const char *file, int line, const char *function)
	: _file(file), _line(line), _function(function) {
		if(!enableTrace)
			return;
		frigg::infoLogger() << "trace: Enter scope "
				<< _file << ":" << _line << " (in function "
				<< _function << ")" << frigg::endLog;
	}

	~ScopeTrace() {
		if(!enableTrace)
			return;
		frigg::infoLogger() << "trace: Exit scope" << frigg::endLog;
	}

private:
	const char *_file;
	int _line;
	const char *_function;
};

#define SCOPE_TRACE() ScopeTrace(__FILE__, __LINE__, __FUNCTION__)

namespace {

unsigned int threadId() {
	return 1;
}

} // anonymous namespace

static constexpr unsigned int mutexRecursive = 1;

// TODO: either use uint32_t or determine the bit based on sizeof(int).
static constexpr unsigned int waiters = 1 << 31;

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

int pthread_attr_getguardsize(const pthread_attr_t *__restrict, size_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_attr_setguardsize(pthread_attr_t *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// pthread functions.
int pthread_create(pthread_t *__restrict, const pthread_attr_t *__restrict,
		void *(*) (void *), void *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pthread_t pthread_self(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
			values(frigg::DefaultHasher<int>(), getAllocator()) { }

	pthread_mutex_t mutex;

	// TODO: this should be unsigned int.
	frigg::Hashmap<int, void *, frigg::DefaultHasher<int>,
			MemoryAllocator> values;
};

int pthread_key_create(pthread_key_t *key, void (*destructor) (void *)) {
	SCOPE_TRACE();

	*key = frigg::construct<__mlibc_key_data>(getAllocator());
	return 0;
}
int pthread_key_delete(pthread_key_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *pthread_getspecific(pthread_key_t key) {
	SCOPE_TRACE();

	if(pthread_mutex_lock(&key->mutex))
		__ensure("Could not lock mutex");
	
	void *value = nullptr;
	auto it = key->values.get(threadId());
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
	
	auto it = key->values.get(threadId());
	if(it) {
		*it = const_cast<void *>(value);
	}else{
		key->values.insert(threadId(), const_cast<void *>(value));
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
			HEL_CHECK(helFutexWake((int *)&once->__mlibc_done));
			return 0;
		}else{
			// a different thread is currently running the initializer.
			__ensure(expected == onceLocked);
			HEL_CHECK(helFutexWait((int *)&once->__mlibc_done, onceLocked));
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
	}else{
		__ensure(type == PTHREAD_MUTEX_NORMAL);
	}
	
	__ensure(robust == PTHREAD_MUTEX_STALLED);
	
	return 0;
}
int pthread_mutex_destroy(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	unsigned int expected = 0;
	while(true) {
		if(!expected) {
			// try to take the mutex here.
			if(__atomic_compare_exchange_n(&mutex->__mlibc_state,
					&expected, 1, false, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE))
				return 0;
		}else{
			if(mutex->__mlibc_flags & mutexRecursive)
				__ensure(!"TODO: Implement recursive mutexes");

			// wait on the futex if the waiters flag is set.
			if(expected & waiters) {
				HEL_CHECK(helFutexWait((int *)&mutex->__mlibc_state, expected));
				
				// opportunistically try to take the lock after we wake up.
				expected = 0;
			}else{
				// otherwise we have to set the waiters flag first.
				unsigned int desired = expected | waiters;
				frigg::infoLogger() << desired << frigg::endLog;
				if(__atomic_compare_exchange_n((int *)&mutex->__mlibc_state,
						&expected, desired, false, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
					expected = desired;
			}
		}
	}
}

int pthread_mutex_trylock(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutex_timedlock(pthread_mutex_t *__restrict,
		const struct timespec *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	SCOPE_TRACE();

	// reset the mutex to the unlocked state.
	auto state = __atomic_exchange_n(&mutex->__mlibc_state, 0, __ATOMIC_RELEASE);
	__ensure((state & ~waiters) == 1);

	// wake the futex if there were waiters.
	if(state & waiters)
		HEL_CHECK(helFutexWake((int *)&mutex->__mlibc_state));
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_condattr_destroy(pthread_condattr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_condattr_getclock(const pthread_condattr_t *__restrict, clockid_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_condattr_setclock(pthread_condattr_t *, clockid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_cond_init(pthread_cond_t *__restrict, const pthread_condattr_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	HEL_CHECK(helFutexWait(&cond->__mlibc_seq, seq));
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
	HEL_CHECK(helFutexWake((int *)&cond->__mlibc_seq));
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



