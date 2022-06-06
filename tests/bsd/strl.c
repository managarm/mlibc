#include <assert.h>
#include <string.h>

int main() {
	char buf[256];

	/* Test copy to 0-sized buffer . */
	memset(buf, 0, sizeof(buf));
	assert(strlcpy(buf, "xyz", 0) == 3);
	assert(strcmp(buf, "") == 0);

	/* Test normal copy. */
	memset(buf, 0, sizeof(buf));
	assert(strlcpy(buf, "xyz", sizeof(buf)) == 3);
	assert(strcmp(buf, "xyz") == 0);

	/* Test truncated copy. */
	memset(buf, 0, sizeof(buf));
	assert(strlcpy(buf, "abcdefabcdef", 10) == 12);
	assert(strcmp(buf, "abcdefabc") == 0);

	/* Test concat to 0-sized buffer. */
	memset(buf, 0, sizeof(buf));
	assert(strlcat(buf, "abc", 0) == 3);
	assert(strcmp(buf, "") == 0);

	/* Test concat to full buffer. */
	memset(buf, 0, sizeof(buf));
	assert(strlcat(buf, "abcde", 6) == 5);
	assert(strcmp(buf, "abcde") == 0);
	assert(strlcat(buf, "xyz", 5) == 8);
	assert(strcmp(buf, "abcde") == 0);

	/* Test normal concat. */
	memset(buf, 0, sizeof(buf));
	assert(strlcat(buf, "abc", sizeof(buf)) == 3);
	assert(strcmp(buf, "abc") == 0);
	assert(strlcat(buf, "xyz", sizeof(buf)) == 6);
	assert(strcmp(buf, "abcxyz") == 0);

	/* Test truncated concat. */
	memset(buf, 0, sizeof(buf));
	assert(strlcat(buf, "abcabc", 10) == 6);
	assert(strcmp(buf, "abcabc") == 0);
	assert(strlcat(buf, "xyzxyz", 10) == 12);
	assert(strcmp(buf, "abcabcxyz") == 0);

	/* Test truncated concat w/ truncated dst. */
	memset(buf, 0, sizeof(buf));
	assert(strlcat(buf, "abcabc", 10) == 6);
	assert(strcmp(buf, "abcabc") == 0);
	assert(strlcat(buf, "xyz", 4) == 7);
	assert(strcmp(buf, "abcabc") == 0);

	return 0;
}
