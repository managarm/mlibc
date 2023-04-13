#ifndef _THREADS_H
#define _THREADS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/threads.h>

enum {
	mtx_plain,
	mtx_recursive,
	mtx_timed,
};

enum {
	thrd_success,
	thrd_timedout,
	thrd_busy,
	thrd_error,
	thrd_nomem,
};

typedef struct __mlibc_thread_data *thrd_t;
typedef struct __mlibc_mutex mtx_t;
typedef struct __mlibc_cond cnd_t;

typedef int (*thrd_start_t)(void*);

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
int thrd_join(thrd_t thr, int *res);

int mtx_init(mtx_t *mtx, int type);
void mtx_destroy(mtx_t *mtx);
int mtx_lock(mtx_t *mtx);
int mtx_unlock(mtx_t *mtx);

int cnd_init(cnd_t *cond);
void cnd_destroy(cnd_t *cond);
int cnd_broadcast(cnd_t *cond);
int cnd_wait(cnd_t *cond, mtx_t *mtx);

#ifdef __cplusplus
}
#endif

#endif /* _THREADS_H */

