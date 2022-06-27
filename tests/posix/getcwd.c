#include <assert.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	char buf[PATH_MAX];
	char *ret = getcwd(buf, PATH_MAX);

	assert(ret);
	assert((strlen(ret) == strlen(buf)) && strlen(ret));
	assert(!strcmp(ret, buf));

	char *ret2 = getcwd(NULL, 0);
	assert(ret2);
	assert(strlen(ret2));
	assert(!strcmp(ret, ret2));
	free(ret2);

	return 0;
}
