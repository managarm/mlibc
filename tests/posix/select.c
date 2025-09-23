#include <assert.h>
#include <limits.h>
#include <sys/select.h>

int main() {
	fd_set fds;
	FD_ZERO(&fds);

	assert(!FD_ISSET(0, &fds));
	assert(!FD_ISSET(1, &fds));
	assert(!FD_ISSET(FD_SETSIZE - 1, &fds));

	FD_SET(0, &fds);
	assert(fds.fds_bits[0] == 1);
	FD_SET(1, &fds);
	assert(fds.fds_bits[0] == 3);
	FD_SET(FD_SETSIZE - 1, &fds);
	assert(fds.fds_bits[(FD_SETSIZE / (CHAR_BIT * sizeof(fds.fds_bits[0]))) - 1] == (typeof(fds.fds_bits[0])) (1UL << ((sizeof(fds.fds_bits[0]) * 8) - 1)));

	assert(FD_ISSET(0, &fds));
	assert(FD_ISSET(1, &fds));
	assert(FD_ISSET(FD_SETSIZE - 1, &fds));

	FD_CLR(1, &fds);
	assert(fds.fds_bits[0] == 1);

	assert(FD_ISSET(0, &fds));
	assert(!FD_ISSET(1, &fds));
	assert(FD_ISSET(FD_SETSIZE - 1, &fds));

	return 0;
}
