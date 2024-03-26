#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>

#define assert_perror(x)            \
	({                          \
		int res = (x);      \
		if (res < 0) {      \
			perror(#x); \
			return 1;   \
		}                   \
		res;                \
	})

#define soft_assert(x, m)                                                   \
	({                                                                  \
		if (!(x)) {                                                 \
			fprintf(stderr, "cond \"%s\" failed: %s\n", #x, m); \
			return 1;                                           \
		}                                                           \
	})

void remove_tmp(const char (*fname)[]) {
	unlink(&(*fname)[0]);
}
#define _cleanup_file_ __attribute__((cleanup(remove_tmp)))

int main(void) {
	int ret;
	char buf[32] = { 0 };
	_cleanup_file_ char filename[] = "xattr_test.XXXXXX";
	_cleanup_file_ char filename2[] = "xattr_test.XXXXXX.2";

	int tmpfile = assert_perror(mkstemp(filename));
	memcpy(filename2, filename, sizeof(filename) - 1);
	assert_perror(symlink(filename, filename2));

	assert_perror(setxattr(filename, "user.T1", "ABC", 3, XATTR_CREATE));
	assert_perror(fsetxattr(tmpfile, "user.T2", "DEF", 3, XATTR_CREATE));

	// for testing remove
	assert_perror(fsetxattr(tmpfile, "user.T3", "DEF", 3, XATTR_CREATE));

	if ((ret = getxattr(filename, "user.T1", buf, 3)) != 3) {
		if (ret < 0) {
			perror("getxattr");
			return 1;
		}

		soft_assert(memcmp(buf, "ABC", 3) == 0, "xattr read wrong");
	}

	ret = lgetxattr(filename2, "user.T1", buf, 3);
	soft_assert(ret < 0 && errno == ENODATA, "lgetxattr deref'd");

	if ((ret = fgetxattr(tmpfile, "user.T3", buf, 3)) != 3) {
		if (ret < 0) {
			perror("fgetxattr");
			return 1;
		}

		soft_assert(memcmp(buf, "DEF", 3) == 0, "xattr read wrong");
	}

	assert_perror(removexattr(filename, "user.T2"));
	assert_perror(fremovexattr(tmpfile, "user.T3"));

	ret = assert_perror(listxattr(filename, buf, sizeof(buf) - 1));
	soft_assert(memmem(buf, ret, "user.T1", 7), "user.T1 not found");
	soft_assert(!memmem(buf, ret, "user.T2", 7), "user.T2 found");
	soft_assert(!memmem(buf, ret, "user.T3", 7), "user.T3 found");

	ret = assert_perror(flistxattr(tmpfile, buf, sizeof(buf) - 1));
	soft_assert(memmem(buf, ret, "user.T1", 7), "user.T1 not found");
	soft_assert(!memmem(buf, ret, "user.T2", 7), "user.T2 found");
	soft_assert(!memmem(buf, ret, "user.T3", 7), "user.T3 found");

	return 0;
}
