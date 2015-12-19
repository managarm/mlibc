
#include <sys/stat.h>

#include <mlibc/ensure.h>

int chmod(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchmod(int, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchmodat(int, const char *, mode_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fstat() is implemented by the platform
int fstatat(int, const char *__restrict, struct stat *__restrict, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int futimens(int fd, const struct timespec times[2]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int lstat(const char *__restrict, struct stat *__restrict) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int mkdir(const char *, mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
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
// stat() is provided by the platform
mode_t umask(mode_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int utimensat(int, const char *, const struct timespec times[2], int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

