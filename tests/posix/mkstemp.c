#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>

void validate_pattern(char *p) {
	assert(memcmp(p, "XXXXXX", 6));
	assert(memchr(p, 0, 6) == NULL);

	for (int i = 0; i < 6; i++) {
		assert(isalnum(p[i]));
	}
}

int main() {
	int ret;

	// Make sure the patterns themselves cannot be chosen.  This
	// *could* happen on glibc, or if we widen the character set
	// used for generating random names. Odds are 1 in 60 billion,
	// but I'd rather not worry about this.
	ret = open("prefixXXXXXX", O_RDWR | O_CREAT | O_EXCL, 0600);
	assert(ret >= 0 || (ret == -1 && errno == EEXIST));
	ret = open("longprefixXXXXXXlongsuffix", O_RDWR | O_CREAT | O_EXCL, 0600);
	assert(ret >= 0 || (ret == -1 && errno == EEXIST));

	ret = mkstemp("short");
	assert(ret == -1);
	assert(errno == EINVAL);

	ret = mkstemp("lessthan6XXX");
	assert(ret == -1);
	assert(errno == EINVAL);

	ret = mkstemps("lessthan6XXXswithsuffix", 11);
	assert(ret == -1);
	assert(errno == EINVAL);

	char *p = strdup("prefixXXXXXX");
	ret = mkstemp(p);
	// We can't really protect against EEXIST...
	assert(ret >= 0 || (ret == -1 && errno == EEXIST));
	assert(!memcmp(p, "prefix", 6));
	assert(p[12] == 0);
	validate_pattern(p + 6);

	if (ret >= 0) {
		ret = close(ret);
		assert(!ret);
	}
	free(p);

	p = strdup("longprefixXXXXXXlongsuffix");
	ret = mkstemps(p, 10);
	// We can't really protect against EEXIST...
	assert(ret >= 0 || (ret == -1 && errno == EEXIST));
	assert(!memcmp(p, "longprefix", 10));
	assert(!memcmp(p + 16, "longsuffix", 10));
	assert(p[26] == 0);
	validate_pattern(p + 10);

	if (ret >= 0) {
		ret = close(ret);
		assert(!ret);
	}
	free(p);

	return 0;
}
