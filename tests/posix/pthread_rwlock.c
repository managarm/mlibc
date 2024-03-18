#include <assert.h>
#include <errno.h>
#include <pthread.h>

static void test_write_lock_unlock() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_wrlock(&rw);
	assert(!res);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_read_lock_unlock() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_rdlock(&rw);
	assert(!res);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_write_trylock_unlock() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_trywrlock(&rw);
	assert(!res);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_read_trylock_unlock() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_tryrdlock(&rw);
	assert(!res);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_write_prevents_read() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_wrlock(&rw);
	assert(!res);
	res = pthread_rwlock_tryrdlock(&rw);
	assert(res == EBUSY);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_write_prevents_write() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_wrlock(&rw);
	assert(!res);
	res = pthread_rwlock_trywrlock(&rw);
	assert(res == EBUSY);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_read_prevents_write() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_rdlock(&rw);
	assert(!res);
	res = pthread_rwlock_trywrlock(&rw);
	assert(res == EBUSY);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_read_allows_read() {
	int res;
	pthread_rwlock_t rw = PTHREAD_RWLOCK_INITIALIZER;
	res = pthread_rwlock_rdlock(&rw);
	assert(!res);
	res = pthread_rwlock_tryrdlock(&rw);
	assert(!res);
	res = pthread_rwlock_unlock(&rw);
	assert(!res);
}

static void test_attr() {
	pthread_rwlockattr_t attr;
	pthread_rwlockattr_init(&attr);

	int pshared;
	pthread_rwlockattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_PRIVATE);

	pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_rwlockattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_SHARED);

	pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
	pthread_rwlockattr_getpshared(&attr, &pshared);
	assert(pshared == PTHREAD_PROCESS_PRIVATE);

	pthread_rwlockattr_destroy(&attr);
}

int main() {
	test_write_lock_unlock();
	test_read_lock_unlock();
	test_write_trylock_unlock();
	test_read_trylock_unlock();
	test_write_prevents_read();
	test_write_prevents_write();
	test_read_prevents_write();
	test_read_allows_read();
	test_attr();

	return 0;
}
