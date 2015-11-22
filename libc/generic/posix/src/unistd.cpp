
#include <unistd.h>

#include <mlibc/ensure.h>

int access(const char *path, int mode) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
int close(int fd) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
int dup2(int src_fd, int dest_fd) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
int execve(const char *, char *const [], char *const []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
int fchown(int fd, uid_t uid, gid_t gid, int mode) {
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
pid_t fork(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
gid_t getegid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uid_t geteuid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
gid_t getgid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int getgroups(int, gid_t []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long gethostid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int gethostname(char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
pid_t getpid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t getppid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
pid_t getsid(pid_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
uid_t getuid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
int isatty(int fd) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
off_t lseek(int, off_t, int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
ssize_t read(int, void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
unsigned int sleep(unsigned int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
char *ttyname(int) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
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
ssize_t write(int, const void *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

