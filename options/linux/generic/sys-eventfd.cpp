#include <sys/eventfd.h>
#include <errno.h>

#include <bits/ensure.h>
#include <mlibc/linux-sysdeps.hpp>

int eventfd(unsigned int initval, int flags) {
	int fd = 0;

	MLIBC_CHECK_OR_ENOSYS(mlibc::sys_eventfd_create, -1);
	if (int e = mlibc::sys_eventfd_create(initval, flags, &fd); e) {
		errno = e;
		return -1;
	}

	return fd;
}

int eventfd_read(int fd, eventfd_t *value) {
	ssize_t bytes_read;
	if (int e = mlibc::sys_read(fd, value, 8, &bytes_read); e) {
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
	if (int e = mlibc::sys_write(fd, &value, 8, &bytes_written); e) {
		errno = e;
		return -1;
	}

	if (bytes_written != 8) {
		return -1;
	}

	return 0;
}
