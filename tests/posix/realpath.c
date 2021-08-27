#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void prepare() {
	assert(!mkdir("/tmp/mlibc-realpath/", S_IRWXU));
	assert(!mkdir("/tmp/mlibc-realpath/dir1", S_IRWXU));
	assert(!mkdir("/tmp/mlibc-realpath/dir2", S_IRWXU));
	assert(!symlink("/tmp/mlibc-realpath/dir2/", "/tmp/mlibc-realpath/dir1/abs-link"));
	chdir("/tmp/mlibc-realpath/dir1");
	assert(!symlink("../dir2/", "/tmp/mlibc-realpath/dir1/rel-link"));
}

void cleanup() {
	assert(!unlink("/tmp/mlibc-realpath/dir1/rel-link"));
	assert(!unlink("/tmp/mlibc-realpath/dir1/abs-link"));
	assert(!rmdir("/tmp/mlibc-realpath/dir2"));
	assert(!rmdir("/tmp/mlibc-realpath/dir1"));
	assert(!rmdir("/tmp/mlibc-realpath/"));
}

void signal_handler(int sig, siginfo_t *info, void *ctx) {
	cleanup();

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = SIG_DFL;
	sa.sa_flags = 0;

	sigaction(SIGABRT, &sa, NULL);
	abort();
}

int main() {
	char *path;

	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = signal_handler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGABRT, &sa, NULL);

	prepare();

	path = realpath("/tmp/mlibc-realpath/dir1/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir1"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/../dir2", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/abs-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/rel-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/abs-link/../", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/rel-link/../", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/abs-link/../dir1/abs-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/rel-link/../dir1/rel-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/abs-link/../dir1/rel-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	path = realpath("/tmp/mlibc-realpath/dir1/rel-link/../dir1/abs-link/", NULL);
	assert(path);
	assert(!strcmp(path, "/tmp/mlibc-realpath/dir2"));
	free(path);

	cleanup();
}
