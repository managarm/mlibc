#ifndef _THREADS_H
#define _THREADS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/threads.h>

enum {
	mtx_plain,
	mtx_recursive,
	mtx_timed
};

enum {
	thrd_success,
	thrd_timedout,
	thrd_busy,
	thrd_error,
	thrd_nomem
};

typedef struct __mlibc_thread_data *thrd_t;
typedef struct __mlibc_mutex mtx_t;
typedef struct __mlibc_cond cnd_t;
#ifndef __cplusplus
#define thread_local _Thread_local
#endif

typedef int (*thrd_start_t)(void* __arg);

#ifndef __MLIBC_ABI_ONLY

int thrd_create(thrd_t *__thr, thrd_start_t __func, void *__arg);
int thrd_equal(thrd_t __lhs, thrd_t __rhs);
thrd_t thrd_current(void);
int thrd_sleep(const struct timespec *__duration, struct timespec *__remaining);
void thrd_yield(void);
int thrd_detach(thrd_t __thr);
int thrd_join(thrd_t __thr, int *__res);
__attribute__((__noreturn__)) void thrd_exit(int __res);

int mtx_init(mtx_t *__mtx, int __type);
void mtx_destroy(mtx_t *__mtx);
int mtx_lock(mtx_t *__mtx);
int mtx_unlock(mtx_t *__mtx);

int cnd_init(cnd_t *__cond);
void cnd_destroy(cnd_t *__cond);
int cnd_broadcast(cnd_t *__cond);
int cnd_wait(cnd_t *__cond, mtx_t *__mtx);

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _THREADS_H */

