#include <assert.h>
#include <locale.h>
#include <stdlib.h>

int main() {
	assert(rpmatch("y") == 1);
	assert(rpmatch("Y") == 1);
	assert(rpmatch("n") == 0);
	assert(rpmatch("N") == 0);
	assert(rpmatch("yno") == 1);
	assert(rpmatch("no") == 0);
	assert(rpmatch("NO") == 0);
	assert(rpmatch("YES") == 1);
	assert(rpmatch("mlibc") == -1);

	return 0;
}
