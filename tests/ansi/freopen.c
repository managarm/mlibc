#include <assert.h>
#include <stdio.h>
#include <string.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "freopen-host-libc.tmp"
#else
#define TEST_FILE "freopen.tmp"
#endif

int main() {
	FILE *file = fopen(TEST_FILE, "w");
	assert(file);

	assert(freopen("/dev/null", "w", file));

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

	return 0;
}
