#include <assert.h>
#include <pthread.h>
#include <errno.h>

_Atomic int dtors_entered = 0;
pthread_key_t key3;

static void dtor1(void *arg) {
	(void)arg;
	dtors_entered++;

	// Set the key during destruction to trigger dtor2 (as it only runs if
	// the key value is non-NULL).
	assert(!pthread_setspecific(key3, &key3));
	assert(pthread_getspecific(key3) == &key3);
}

static void dtor2(void *arg) {
	(void)arg;
	dtors_entered++;
}

static void *worker(void *arg) {
	(void)arg;

	pthread_key_t key1, key2;
	assert(!pthread_key_create(&key1, NULL));
	assert(!pthread_key_create(&key2, dtor1));
	assert(!pthread_key_create(&key3, dtor2));

	assert(!pthread_setspecific(key1, &key1));
	assert(pthread_getspecific(key1) == &key1);

	assert(!pthread_setspecific(key2, &key2));
	assert(pthread_getspecific(key2) == &key2);

	pthread_exit(0);
	return NULL;
}

int main() {
	// NOTE that the EINVAL return from pthread_setspecific is mlibc-specific,
	// POSIX specifies that accessing an invalid key is undefined behavior.

	assert(pthread_getspecific(PTHREAD_KEYS_MAX) == NULL);
	assert(pthread_setspecific(PTHREAD_KEYS_MAX, NULL) == EINVAL);

	pthread_key_t key;
	assert(!pthread_key_create(&key, NULL));
	assert(!pthread_setspecific(key, &key));
	assert(pthread_getspecific(key) == &key);
	assert(!pthread_key_delete(key));

	pthread_t thread;
	assert(!pthread_create(&thread, NULL, &worker, NULL));
	assert(!pthread_join(thread, NULL));

	assert(pthread_getspecific(key) == NULL);
	assert(!pthread_setspecific(key, &key));
	assert(pthread_getspecific(key) == &key);

	assert(dtors_entered == 2);

	return 0;
}
