#pragma once

#include <mlibc/sysdeps-interface.hpp>

namespace mlibc {

struct Sysdeps : public mlibc::AnsiCompatSysdeps {
	void exit(int status) {
		sys_exit(status);
	}

	void thread_exit() {
		sys_thread_exit();
	}

	int futex_wait(int *pointer, int expected, const struct timespec *time) {
		return sys_futex_wait(pointer, expected, time);
	}

	int futex_wake(int *pointer) {
		return sys_futex_wake(pointer);
	}

    int open(const char *path, int flags, mode_t mode, int *fd) {
		return sys_open(path, flags, mode, fd);
	}

    int write(int fd, const void *buf, size_t count, ssize_t *bytes_written) {
		return sys_write(fd, buf, count, bytes_written);
	}

    int read(int fd, void *buf, size_t count, ssize_t *bytes_read) {
		return sys_read(fd, buf, count, bytes_read);
	}

	int seek(int fd, off_t offset, int whence, off_t *new_offset) {
		return sys_seek(fd, offset, whence, new_offset);
	}

	int close(int fd) {
		return sys_close(fd);
	}

	int clock_get(int clock, time_t *secs, long *nanos) {
		return sys_clock_get(clock, secs, nanos);
	}

	pid_t getpid() {
		return sys_getpid();
	}

	int sigprocmask(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve);
};

} // namespace mlibc
