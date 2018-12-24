
#include <errno.h>
#include <bits/ensure.h>
#include <sys/stat.h>

#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

int chmod(const char *, mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: chmod() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}
int fchmod(int, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchmodat(int, const char *, mode_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fstatat(int, const char *__restrict, struct stat *__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int futimens(int fd, const struct timespec times[2]) {
	mlibc::infoLogger() << "\e[31mmlibc: futimens() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}
int mkdir(const char *path, mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: mkdir() ignore the mode\e[39m" << frg::endlog;
	if(int e = mlibc::sys_mkdir(path); e) {
		errno = e;
		return -1;
	}
	return 0;
}
int mkdirat(int, const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mkfifo(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mkfifoat(int, const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mknod(const char *, mode_t, dev_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mknodat(const char *, mode_t, dev_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
mode_t umask(mode_t) {
	mlibc::infoLogger() << "\e[31mmlibc: umask() is a no-op and always returns 0\e[39m"
			<< frg::endlog;
	return 0;
}
int utimensat(int, const char *, const struct timespec times[2], int) {
	mlibc::infoLogger() << "\e[31mmlibc: utimensat() is not implemented correctly\e[39m"
			<< frg::endlog;
	return 0;
}


int stat(const char *__restrict path, struct stat *__restrict result) {
	if(int e = mlibc::sys_stat(path, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int lstat(const char *__restrict path, struct stat *__restrict result) {
	if(int e = mlibc::sys_lstat(path, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

int fstat(int fd, struct stat *result) {
	if(int e = mlibc::sys_fstat(fd, result); e) {
		errno = e;
		return -1;
	}
	return 0;
}

