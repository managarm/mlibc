#include <assert.h>
#include <stdio.h>
#include <string.h>

// This test is also built for the ANSI-only mlibc configuration. In that
// configuration neither fileno() nor unlink() is available.
#if defined(USE_HOST_LIBC) || __MLIBC_POSIX_OPTION
#include <errno.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <unistd.h>
#define TEST_HAS_POSIX 1
#else
#define TEST_HAS_POSIX 0
#endif

#ifdef USE_HOST_LIBC
#define TEST_FILE "freopen-host-libc.tmp"
#elif defined(USE_CROSS_LIBC)
#define TEST_FILE "freopen-cross-libc.tmp"
#else
#define TEST_FILE "freopen.tmp"
#endif

#if TEST_HAS_POSIX
static void test_descriptor_limit(void) {
	struct rlimit saved_limit;
	assert(getrlimit(RLIMIT_NOFILE, &saved_limit) == 0);

	FILE *file = fopen("/dev/null", "w");
	assert(file);

	struct rlimit limited = saved_limit;
	limited.rlim_cur = fileno(file) + 1;
	assert(setrlimit(RLIMIT_NOFILE, &limited) == 0);

	errno = 0;
	assert(freopen("/dev/null", "w", file) == NULL);
	assert(errno == EMFILE);

	assert(setrlimit(RLIMIT_NOFILE, &saved_limit) == 0);
}

static void test_close_on_exec(void) {
	FILE *file = fopen("/dev/null", "w");
	assert(file);
	assert(freopen("/dev/null", "we", file));
	assert(fcntl(fileno(file), F_GETFD) & FD_CLOEXEC);
	assert(fclose(file) == 0);
}
#endif

int main() {
	// POSIX requires freopen() to ignore errors while flushing the old stream.
	FILE *full = fopen("/dev/full", "w");
	assert(full);
	assert(fwrite("x", 1, 1, full) == 1);
	assert(freopen("/dev/null", "w", full));
	assert(fclose(full) == 0);

#if TEST_HAS_POSIX
	test_descriptor_limit();
	test_close_on_exec();
#endif

	FILE *file = fopen(TEST_FILE, "w");
	assert(file);

#if TEST_HAS_POSIX
	int original_fd = fileno(file);
#endif
	assert(freopen("/dev/null", "w", file));

#if TEST_HAS_POSIX
	assert(fileno(file) == original_fd);
#endif

	char str[] = "mlibc freopen test";
	fwrite(str, 1, sizeof(str) - 1, file);
	fflush(file);
	fclose(file);

	file = fopen(TEST_FILE, "r");
	assert(file);

	char buf[sizeof(str)];
	memset(buf, 0, sizeof(buf));
	int ret = fread(buf, 1, sizeof(buf) - 1, file);
	fprintf(stderr, "ret %d\n", ret);
	assert(ret == 0);
	fclose(file);

	file = fopen("/dev/null", "w");
	assert(file);

	assert(freopen(TEST_FILE, "w", file));
	fwrite(str, 1, sizeof(str) - 1, file);
	fflush(file);
	fclose(file);

	memset(buf, 0, sizeof(buf));
	file = fopen(TEST_FILE, "r");
	assert(fread(buf, 1, sizeof(buf) - 1, file));

	fprintf(stderr, "buffer content '%s'\n", buf);
	assert(!strcmp(buf, "mlibc freopen test"));
	fclose(file);

	assert(remove(TEST_FILE) == 0);

	return 0;
}
