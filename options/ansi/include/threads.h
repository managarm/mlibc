#ifndef _THREADS_H
#define _THREADS_H

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct __mlibc_thread_data * thrd_t;
typedef struct __mlibc_mutex mtx_t;
typedef struct __mlibc_cond cnd_t;

#ifdef __cplusplus
}
#endif

#endif /* _THREADS_H */

