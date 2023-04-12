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

#ifdef __cplusplus
}
#endif

#endif /* _THREADS_H */

