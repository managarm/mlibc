#include <aio.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "aio_test_file-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "aio_test_file-cross-libc.tmp"
#else
#define TEST_FILE "aio_test_file.tmp"
#endif

#define TEST_STR "Hello, POSIX AIO!"
#define TEST_STR_LEN (sizeof(TEST_STR) - 1)

static void wait_for_aio(struct aiocb *cb) {
	const struct aiocb *list[1] = {cb};
	while (aio_error(cb) == EINPROGRESS) {
		int res = aio_suspend(list, 1, NULL);
		assert(res == 0 || (res == -1 && errno == EINTR));
	}
}

static void test_round_trip() {
	int fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
	assert(fd >= 0);

	struct aiocb cb;
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)TEST_STR;
	cb.aio_nbytes = TEST_STR_LEN;
	cb.aio_offset = 0;

	assert(aio_write(&cb) == 0);
	wait_for_aio(&cb);
	assert(aio_error(&cb) == 0);
	assert(aio_return(&cb) == (ssize_t)TEST_STR_LEN);

	char buf[TEST_STR_LEN + 1];
	memset(buf, 0, sizeof(buf));
	cb.aio_buf = buf;
	cb.aio_offset = 0;
	assert(aio_read(&cb) == 0);
	wait_for_aio(&cb);
	assert(aio_error(&cb) == 0);
	assert(aio_return(&cb) == (ssize_t)TEST_STR_LEN);
	assert(memcmp(buf, TEST_STR, TEST_STR_LEN) == 0);

	close(fd);
	unlink(TEST_FILE);
}

static pthread_mutex_t lio_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t lio_cond = PTHREAD_COND_INITIALIZER;
static int lio_finished = 0;

static void lio_callback(union sigval val) {
	(void)val;
	pthread_mutex_lock(&lio_mutex);
	lio_finished = 1;
	pthread_cond_signal(&lio_cond);
	pthread_mutex_unlock(&lio_mutex);
}

static void test_lio() {
	int fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
	assert(fd >= 0);

	struct aiocb cb1, cb2;
	struct aiocb *list[2] = {&cb1, &cb2};

	memset(&cb1, 0, sizeof(cb1));
	cb1.aio_fildes = fd;
	cb1.aio_buf = (void *)TEST_STR;
	cb1.aio_nbytes = TEST_STR_LEN;
	cb1.aio_offset = 0;
	cb1.aio_lio_opcode = LIO_WRITE;

	memset(&cb2, 0, sizeof(cb2));
	cb2.aio_fildes = fd;
	cb2.aio_buf = (void *)TEST_STR;
	cb2.aio_nbytes = TEST_STR_LEN;
	cb2.aio_offset = TEST_STR_LEN;
	cb2.aio_lio_opcode = LIO_WRITE;

	// LIO_WAIT
	assert(lio_listio(LIO_WAIT, list, 2, NULL) == 0);
	assert(aio_error(&cb1) == 0);
	assert(aio_error(&cb2) == 0);
	assert(aio_return(&cb1) == (ssize_t)TEST_STR_LEN);
	assert(aio_return(&cb2) == (ssize_t)TEST_STR_LEN);

	// LIO_NOWAIT with SIGEV_NONE
	cb1.aio_offset = 2 * TEST_STR_LEN;
	cb2.aio_offset = 3 * TEST_STR_LEN;
	assert(lio_listio(LIO_NOWAIT, list, 2, NULL) == 0);
	wait_for_aio(&cb1);
	wait_for_aio(&cb2);
	assert(aio_return(&cb1) == (ssize_t)TEST_STR_LEN);
	assert(aio_return(&cb2) == (ssize_t)TEST_STR_LEN);

	// LIO_NOWAIT with SIGEV_THREAD
	struct sigevent sev;
	memset(&sev, 0, sizeof(sev));
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = lio_callback;

	cb1.aio_offset = 4 * TEST_STR_LEN;
	cb2.aio_offset = 5 * TEST_STR_LEN;
	lio_finished = 0;
	assert(lio_listio(LIO_NOWAIT, list, 2, &sev) == 0);

	pthread_mutex_lock(&lio_mutex);
	while (!lio_finished) {
		pthread_cond_wait(&lio_cond, &lio_mutex);
	}
	pthread_mutex_unlock(&lio_mutex);

	assert(aio_error(&cb1) == 0);
	assert(aio_error(&cb2) == 0);
	assert(aio_return(&cb1) == (ssize_t)TEST_STR_LEN);
	assert(aio_return(&cb2) == (ssize_t)TEST_STR_LEN);

	close(fd);
	unlink(TEST_FILE);
}

static void test_cancel() {
	int fds[2];
	assert(pipe(fds) == 0);

	struct aiocb cb;
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = fds[0];
	void *ptr = malloc(1);
	cb.aio_buf = ptr;
	cb.aio_nbytes = 1;

	assert(aio_read(&cb) == 0);
	int cres = aio_cancel(fds[0], &cb);
	assert(cres == AIO_CANCELED || cres == AIO_ALLDONE || cres == AIO_NOTCANCELED);

	if (cres != AIO_CANCELED)
		close(fds[1]);

	wait_for_aio(&cb);

	if (cres == AIO_CANCELED) {
		assert(aio_error(&cb) == ECANCELED);
	}

	aio_return(&cb);
	free(ptr);
	close(fds[0]);
	if (cres == AIO_CANCELED)
		close(fds[1]);

	// Cancel per-fd
	int fd = open(TEST_FILE, O_RDWR | O_CREAT | O_TRUNC, 0666);
	assert(fd >= 0);
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = fd;
	cb.aio_buf = (void *)TEST_STR;
	cb.aio_nbytes = TEST_STR_LEN;
	cb.aio_offset = 0;
	assert(aio_write(&cb) == 0);
	int res = aio_cancel(fd, NULL);
	assert(res == AIO_CANCELED || res == AIO_NOTCANCELED || res == AIO_ALLDONE);

	wait_for_aio(&cb);
	aio_return(&cb);

	close(fd);
	unlink(TEST_FILE);
}

static void test_suspend() {
	int fds[2];
	assert(pipe(fds) == 0);

	struct aiocb cb;
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = fds[0];
	void *ptr = malloc(1);
	cb.aio_buf = ptr;
	cb.aio_nbytes = 1;

	assert(aio_read(&cb) == 0);

	const struct aiocb *list[1] = {&cb};
	struct timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 100000000; // 100ms

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int res = aio_suspend(list, 1, &timeout);
	clock_gettime(CLOCK_MONOTONIC, &end);

	assert(res == -1);
	assert(errno == EAGAIN);

	long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
	assert(elapsed_ms >= 100); // Allow some margin for timing.

	aio_cancel(fds[0], &cb);
	close(fds[1]);
	wait_for_aio(&cb);
	aio_return(&cb);

	close(fds[0]);
	free(ptr);
}

static void test_errors() {
	struct aiocb cb;
	memset(&cb, 0, sizeof(cb));
	cb.aio_fildes = -1;

	assert(aio_fsync(O_SYNC, &cb) == -1);
	assert(errno == EBADF);

	assert(aio_cancel(-1, NULL) == -1);
	assert(errno == EBADF);

	// Test lio_listio with an invalid opcode in list
	struct aiocb cb_invalid;
	memset(&cb_invalid, 0, sizeof(cb_invalid));
	cb_invalid.aio_lio_opcode = -1; // invalid opcode

	struct aiocb cb_valid;
	memset(&cb_valid, 0, sizeof(cb_valid));
	cb_valid.aio_fildes = -1; // invalid fd but valid opcode LIO_WRITE
	cb_valid.aio_lio_opcode = LIO_WRITE;
	cb_valid.aio_nbytes = 10;
	cb_valid.aio_buf = malloc(10);
	cb_valid.aio_offset = 0;

	struct aiocb *list[] = {&cb_invalid, &cb_valid};
	assert(lio_listio(LIO_WAIT, list, 2, NULL) == -1);
	assert(errno == EIO);

	assert(aio_error(&cb_invalid) == EINVAL);
	assert(aio_error(&cb_valid) == EBADF);

	free((void *)cb_valid.aio_buf);
}

int main() {
	// qemu-user does not support io_uring; disable this test for !x86_64 on GHA.
#if !defined(__x86_64__)
	const char *gha = getenv("GITHUB_ACTIONS");

	if (!gha || strcmp(gha, "true")) {
#endif // !defined(__x86_64__)
	test_round_trip();
	test_lio();
	test_cancel();
	test_suspend();
	test_errors();
#if !defined(__x86_64__)
	}
#endif // !defined(__x86_64__)

	return 0;
}
