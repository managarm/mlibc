
#include <string.h>
#include <sys/select.h>
#include <frigg/debug.hpp>
#include <mlibc/ensure.h>

void FD_CLR(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->__mlibc_elems[fd / 8] &= ~(1 << (fd % 8));
}
int FD_ISSET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	return set->__mlibc_elems[fd / 8] & (1 << (fd % 8));
}
void FD_SET(int fd, fd_set *set) {
	__ensure(fd < FD_SETSIZE);
	set->__mlibc_elems[fd / 8] |= 1 << (fd % 8);
}
void FD_ZERO(fd_set *set) {
	memset(set->__mlibc_elems, 0, sizeof(fd_set));
}

int select(int num_fds, fd_set *__restrict read_set, fd_set *__restrict write_set,
		fd_set *__restrict except_set, struct timeval *__restrict timeout) {
	frigg::infoLogger() << "\e[35mmlibc: Broken select() called.\e[39m" << frigg::endLog;

	int num_returned = 0;
	for(int i = 0; i < num_fds; i++) {
		if(except_set && FD_ISSET(i, except_set))
			FD_CLR(i, except_set);
		if(read_set && FD_ISSET(i, read_set))
			num_returned++;
		if(write_set && FD_ISSET(i, write_set))
			num_returned++;
	}
	return num_returned;
}

