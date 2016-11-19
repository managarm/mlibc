
#include <pthread.h>

#include <mlibc/ensure.h>

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

int pthread_key_create(pthread_key_t *, void (*) (void *)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_key_delete(pthread_key_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

void *pthread_getspecific(pthread_key_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_setspecific(pthread_key_t, const void *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// pthread_once functions.
// ----------------------------------------------------------------------------

int pthread_once(pthread_once_t *, void (*) (void)) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// ----------------------------------------------------------------------------
// pthread_mutexattr and pthread_mutex functions.
// ----------------------------------------------------------------------------

// pthread_mutexattr functions
int pthread_mutexattr_init(pthread_mutexattr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutexattr_destroy(pthread_mutexattr_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t *__restrict, int *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutexattr_settype(pthread_mutexattr_t *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
int pthread_mutex_init(pthread_mutex_t *__restrict, const pthread_mutexattr_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_mutex_destroy(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutex_lock(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
int pthread_mutex_unlock(pthread_mutex_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_cond_wait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cond_timedwait(pthread_cond_t *__restrict, pthread_mutex_t *__restrict,
		const struct timespec *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cond_signal(pthread_cond_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cond_broadcast(pthread_cond_t *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


