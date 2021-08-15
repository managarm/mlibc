#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <stdio.h>

struct thread_arg {
	int *counter;
	pthread_cond_t *cond;
	pthread_mutex_t *mtx;
};

static void *waiting_thread(void *arg) {
	struct thread_arg *t = arg;
	pthread_mutex_lock(t->mtx);
	pthread_cond_wait(t->cond, t->mtx);
	++*t->counter;
	pthread_mutex_unlock(t->mtx);
	return NULL;
}

static void test_broadcast_wakes_all() {
	int cnt = 0;
	pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

	pthread_t t1, t2;
	struct thread_arg arg = { .counter = &cnt, .cond = &cond, .mtx = &mtx };
	pthread_create(&t1, NULL, &waiting_thread, &arg);
	pthread_create(&t2, NULL, &waiting_thread, &arg);

	struct timespec wait_time = { .tv_sec = 0, .tv_nsec = 150000000 }; // 150ms
	nanosleep(&wait_time, NULL);

	pthread_mutex_lock(&mtx);
	assert(!pthread_cond_broadcast(&cond));
	pthread_mutex_unlock(&mtx);

	nanosleep(&wait_time, NULL);
	assert(cnt == 2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
}

int main() {
	test_broadcast_wakes_all();
}
