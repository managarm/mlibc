#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

int main() {
	char buf[LOGIN_NAME_MAX];
	int ret;

	ret = getlogin_r(buf, sizeof(buf));
	fprintf(stderr, "ret = %d\n", ret);
	assert(ret == 0 || ret == ENOTTY);
	if (ret == 0)
		printf("user: '%s'\n", buf);

	ret = getlogin_r(buf, 1);
	assert(ret == ERANGE || ret == ENOTTY);

	return 0;
}
