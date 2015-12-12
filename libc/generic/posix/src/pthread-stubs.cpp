
#include <pthread.h>

#include <mlibc/ensure.h>

int pthread_attr_init(pthread_attr_t *attr) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_create(pthread_t *__restrict pthread, const pthread_attr_t *__restrict attr,
		void *(*entry)(void *), void *argument) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cleanup_pop(int exec) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_cleanup_push(void (*handler)(void *), void *argument) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pthread_join(pthread_t pthread, void **result) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
		const pthread_mutex_attr_t *__restrict attr) {
	// TODO: we need proper mutexes
	return 0;
}
int pthread_mutex_lock(pthread_mutex_t *mutex) {
	// TODO: we need proper mutexes
	return 0;
}
int pthread_mutex_unlock(pthread_mutex_t *mutex) {
	// TODO: we need proper mutexes
	return 0;
}
int pthread_mutex_destroy(pthread_mutex_t *mutex) {
	// TODO: we need proper mutexes
	return 0;
}

