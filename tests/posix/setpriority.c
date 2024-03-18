#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

int main() {
	errno = 0;
	int original_priority = getpriority(PRIO_PROCESS, getpid());

	assert(original_priority != -1 || !errno);

	int ret = setpriority(PRIO_PROCESS, getpid(), original_priority + 1);

	if(ret) {
		fprintf(stderr, "%s", strerror(errno));
		exit(1);
	}

	errno = 0;
	int new_priority = getpriority(PRIO_PROCESS, getpid());

	assert(new_priority != -1 || !errno);
	assert(new_priority == original_priority + 1);

	return 0;
}
