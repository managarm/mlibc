#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#ifdef USE_HOST_LIBC
#define TEST_BASE "/tmp/mlibc-realpath-host-libc"
#else
#define TEST_BASE "/tmp/mlibc-realpath"
#endif

void prepare() {
	assert(!mkdir(TEST_BASE "/", S_IRWXU));
	assert(!mkdir(TEST_BASE "/dir1", S_IRWXU));
	assert(!mkdir(TEST_BASE "/dir2", S_IRWXU));
	assert(!symlink(TEST_BASE "/dir2/", TEST_BASE "/dir1/abs-link"));
	assert(!symlink(TEST_BASE "/dir2///", TEST_BASE "/dir1/abs-link-trail"));
	assert(!symlink(TEST_BASE "/dir2", TEST_BASE "/dir1/abs-link-no-trail"));
	assert(!chdir(TEST_BASE "/dir1"));
	assert(!symlink("../dir2/", TEST_BASE "/dir1/rel-link"));
}

void cleanup(int do_assert) {
	if (do_assert) {
		assert(!unlink(TEST_BASE "/dir1/rel-link"));
		assert(!unlink(TEST_BASE "/dir1/abs-link"));
		assert(!unlink(TEST_BASE "/dir1/abs-link-trail"));
		assert(!unlink(TEST_BASE "/dir1/abs-link-no-trail"));
		assert(!rmdir(TEST_BASE "/dir2"));
		assert(!rmdir(TEST_BASE "/dir1"));
		assert(!rmdir(TEST_BASE "/"));
	} else {
		unlink(TEST_BASE "/dir1/rel-link");
		unlink(TEST_BASE "/dir1/abs-link");
		unlink(TEST_BASE "/dir1/abs-link-trail");
		unlink(TEST_BASE "/dir1/abs-link-no-trail");
		rmdir(TEST_BASE "/dir2");
		rmdir(TEST_BASE "/dir1");
		rmdir(TEST_BASE "/");
	}
}

void signal_handler(int sig, siginfo_t *info, void *ctx) {
	(void)sig;
	(void)info;
	(void)ctx;

	cleanup(0);

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;

	sigaction(SIGABRT, &sa, NULL);
	abort();
}

#define TEST_PATH(rpath, expected) \
	path = realpath(rpath, NULL); \
	assert(path); \
	assert(!strcmp(path, expected)); \
	free(path);


int main() {
	char *path;

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &sa, NULL);

	prepare();

	TEST_PATH(TEST_BASE "/dir1/",                           TEST_BASE "/dir1");
	TEST_PATH(TEST_BASE "/dir1/../dir2",                    TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/abs-link/",                  TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/rel-link/",                  TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/abs-link/../",               TEST_BASE "");
	TEST_PATH(TEST_BASE "/dir1/rel-link/../",               TEST_BASE "");
	TEST_PATH(TEST_BASE "/dir1/abs-link/../dir1/abs-link/", TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/rel-link/../dir1/rel-link/", TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/abs-link/../dir1/rel-link/", TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/rel-link/../dir1/abs-link/", TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/abs-link-no-trail/",         TEST_BASE "/dir2");
	TEST_PATH(TEST_BASE "/dir1/abs-link-trail/",            TEST_BASE "/dir2");

	TEST_PATH("/tmp", "/tmp");
	TEST_PATH("/",    "/");
	TEST_PATH("//",   "/");

	cleanup(1);

	return 0;
}
