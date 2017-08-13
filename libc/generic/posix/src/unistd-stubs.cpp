
#include <unistd.h>

#include <mlibc/ensure.h>

// access() is provided by the platform
unsigned int alarm(unsigned int seconds) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int chdir(const char *path) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int chown(const char *path, uid_t uid, gid_t gid) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// close() is provided by the platform
ssize_t confstr(int, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *crypt(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int dup(int fd) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// dup2() is provided by the platform
void _exit(int status) {
	__ensure(!"Not implemented");
}
void encrypt(char block[64], int flags) {
	__ensure(!"Not implemented");
}
int execl(const char *, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int execle(const char *, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int execlp(const char *, const char *, ...) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int execv(const char *, char *const []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// execve() is provided by the platform
int execvp(const char *, char *const[]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int faccessat(int, const char *, int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchdir(int fd) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchown(int fd, uid_t uid, gid_t gid) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fchownat(int fd, const char *path, uid_t uid, gid_t gid, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fdatasync(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fexecve(int, char *const [], char *const []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// fork() is provided by the platform
long fpathconf(int, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int fsync(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int ftruncate(int, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *getcwd(char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// getegid() is provided by the platform
// geteuid() is provided by the platform
// getgid() is provided by the platform
int getgroups(int, gid_t []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long gethostid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// gethostname() is provided by the platform
char *getlogin(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getlogin_r(char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getopt(int, char *const [], const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t getpgid(pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t getpgrp(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// getpid() is provided by the platform
// getppid() is provided by the platform
pid_t getsid(pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// getuid() is provided by the platform
// isatty() is provided by the platform
int lchown(const char *path, uid_t uid, gid_t gid) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int link(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int linkat(int, const char *, int, const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int lockf(int, int, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// lseek() is provided by the platform
int nice(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long pathconf(const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pause(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int pipe(int [2]) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
ssize_t pread(int, void *, size_t, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
ssize_t pwrite(int, const void *, size_t, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// read is provided by the platform
ssize_t readlink(const char *__restrict, char *__restrict, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
ssize_t readlinkat(int, const char *__restrict, char *__restrict, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int rmdir(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setegid(gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int seteuid(uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setgid(gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setpgid(pid_t, pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t setpgrp(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setregid(gid_t, gid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setreuid(uid_t, uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t setsid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int setuid(uid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// sleep() is provided by the platform
void swab(const void *__restrict, void *__restrict, ssize_t) {
	__ensure(!"Not implemented");
}
int symlink(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int symlinkat(const char *, int, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void sync(void) {
	__ensure(!"Not implemented");
}
unsigned long sysconf(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t tcgetpgrp(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int tcsetpgrp(int, pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int truncate(const char *, off_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// ttyname is provided by the platform
int ttyname_r(int, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int unlink(const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int unlinkat(int, const char *, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
// write() is provided by the platform

char *optarg;
int optind;
int opterr;
int optopt;

int getpagesize(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// usleep() is provided by the platform

