
#ifndef _PTHREAD_H
#define _PTHREAD_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: move to own file and include in sys/types.h
struct __mlibc_Thread { };
typedef struct __mlibc_Thread *pthread_t;

struct __mlibc_ThreadAttr { };
typedef struct __mlibc_ThreadAttr pthread_attr_t;

struct  __mlibc_MutexAttr { };
typedef struct __mlibc_MutexAttr pthread_mutex_attr_t;

struct __mlibc_Mutex { };
typedef struct __mlibc_Mutex pthread_mutex_t;

int pthread_attr_init(pthread_attr_t *attr);
int pthread_create(pthread_t *__restrict pthread, const pthread_attr_t *__restrict attr,
		void *(*entry)(void *), void *argument);
int pthread_cleanup_pop(int exec);
int pthread_cleanup_push(void (*handler)(void *), void *argument);
int pthread_join(pthread_t pthread, void **result);

int pthread_mutex_init(pthread_mutex_t *__restrict mutex,
		const pthread_mutex_attr_t *__restrict attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif // _PTHREAD_H

