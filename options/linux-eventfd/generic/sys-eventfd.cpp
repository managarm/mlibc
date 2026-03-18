#include <sys/eventfd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

int eventfd(unsigned int initval, int flags) {
	int fd = 0;
	if (int e = mlibc::sysdep_or_enosys<EventfdCreate>(initval, flags, &fd); e) {
		errno = e;
		return -1;
	}

	return fd;
}

int eventfd_read(int fd, eventfd_t *value) {
	ssize_t bytes_read;
	if (int e = mlibc::sysdep<Read>(fd, value, 8, &bytes_read); e) {
		errno = e;
		return -1;
	}

	if (bytes_read != 8) {
		return -1;
	}

	return 0;
}

int eventfd_write(int fd, eventfd_t value) {
	ssize_t bytes_written;
	if (int e = mlibc::sysdep<Write>(fd, &value, 8, &bytes_written); e) {
		errno = e;
		return -1;
	}

	if (bytes_written != 8) {
		return -1;
	}

	return 0;
}
