#include <sys/resource.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

int main() {
	struct rlimit getlim, setlim;

    setlim.rlim_cur = 16;
    setlim.rlim_max = 4096;

	int ret = setrlimit(RLIMIT_NOFILE, &setlim);

    if(ret == -1) {
        fprintf(stderr, "%s\n", strerror(errno));
		assert(!ret);
	}

    assert(!getrlimit(RLIMIT_NOFILE, &getlim));

	assert(setlim.rlim_cur == getlim.rlim_cur);
	assert(setlim.rlim_max == getlim.rlim_max);

    return 0;
}
