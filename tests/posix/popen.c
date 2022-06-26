#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#ifdef USE_HOST_LIBC
#define TEST_FILE "popen-host-libc.tmp"
#else
#define TEST_FILE "popen.tmp"
#endif

#define TEST_STRING1 "the quick brown fox jumps over the lazy dog"
#define TEST_STRING1_LEN 43
#define TEST_STRING2 "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
#define TEST_STRING2_LEN 55

int main() {
	// Test reading
	FILE *f1 = popen("echo -n '" TEST_STRING1 "'", "r");
	assert(f1);
	char buf1[TEST_STRING1_LEN];
	assert(fread(buf1, 1, TEST_STRING1_LEN, f1) == TEST_STRING1_LEN);
	assert(memcmp(buf1, TEST_STRING1, TEST_STRING1_LEN) == 0);
	pclose(f1);

	// Test writing
	FILE *f2 = popen("cat - > " TEST_FILE, "w");
	assert(f2);
	assert(fwrite(TEST_STRING2, 1, TEST_STRING2_LEN, f2) == TEST_STRING2_LEN);
	pclose(f2);

	// Read test file back
	FILE *test_file = fopen(TEST_FILE, "r");
	assert(test_file);
	char buf2[TEST_STRING2_LEN];
	assert(fread(buf2, 1, TEST_STRING2_LEN, test_file) == TEST_STRING2_LEN);
	assert(memcmp(buf2, TEST_STRING2, TEST_STRING2_LEN) == 0);
	fclose(test_file);
	assert(unlink(TEST_FILE) == 0);

	return 0;
}
