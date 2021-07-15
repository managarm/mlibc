#include <stdio.h>
#include <string.h>
#include <assert.h>

#define TEST_FILE "popen.tmp"
#define TEST_STRING1 "the quick brown fox jumps over the lazy dog"
#define TEST_STRING1_LEN 43
#define TEST_STRING2 "Lorem ipsum dolor sit amet, consectetur adipiscing elit"
#define TEST_STRING2_LEN 55

int main() {
	// Test reading
	FILE *f1 = popen("echo -n '" TEST_STRING1 "'", "r");
	assert(f1);
	char buf1[TEST_STRING1_LEN + 1];
	assert(fread(buf1, 1, TEST_STRING1_LEN, f1) == TEST_STRING1_LEN);
	buf1[TEST_STRING1_LEN] = 0;
	assert(strcmp(buf1, TEST_STRING1) == 0);

	// Test writing
	FILE *f2 = popen("cat - > " TEST_FILE, "w");
	assert(f2);
	assert(fwrite(TEST_STRING2, 1, TEST_STRING2_LEN, f2) == TEST_STRING2_LEN);

	// We need pclose() for further testing
#if 0
	// Read test file back
	FILE *test_file = fopen(TEST_FILE, "r");
	char buf2[TEST_STRING2_LEN + 1];
	assert(fread(buf2, 1, TEST_STRING2_LEN, test_file) == TEST_STRING2_LEN);
	buf2[TEST_STRING2_LEN] = 0;
	assert(strcmp(buf2, TEST_STRING2) == 0);
	fclose(test_file);
	assert(remove(TEST_FILE) == 0);
#endif

	return 0;
}
