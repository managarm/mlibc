#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>
#include <limits.h>

int main() {
	errno = 0;
	int original_priority = nice(0);

	assert(original_priority != -1 || !errno);

	errno = 0;
	int ret = nice(1);

	if(ret == -1 && errno) {
		fprintf(stderr, "%s", strerror(errno));
		exit(1);
	}

	errno = 0;
	int new_priority = nice(0);

	assert(new_priority != -1 || !errno);
	assert(new_priority == original_priority + 1);

	errno = 0;
	new_priority = nice(INT_MAX - new_priority);

	assert(new_priority != -1 || !errno);
	assert(new_priority != INT_MAX);

	return 0;
}
