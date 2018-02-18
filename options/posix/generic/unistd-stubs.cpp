
#include <stdarg.h>
#include <bits/ensure.h>
#include <unistd.h>

#include <frigg/debug.hpp>
#include <mlibc/sysdeps.hpp>

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
ssize_t confstr(int, char *, size_t) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
char *crypt(const char *, const char *) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
void _exit(int status) {
	__ensure(!"Not implemented");
}
void encrypt(char block[64], int flags) {
	__ensure(!"Not implemented");
}
int execl(const char *path, const char *arg0, ...) {
	// TODO: It's a stupid idea to limit the number of arguments here.
	char *argv[16];
	int n = 0;

	va_list args;
	va_start(args, arg0);
	while(true) {
		__ensure(n < 16);
		auto argn = va_arg(args, const char *);
		argv[n++] = const_cast<char *>(argn);
		if(!argn)
			break;
	}
	va_end(args);

	return execve(path, argv, environ);
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
int getgroups(int, gid_t []) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}
long gethostid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int gethostname(char *buffer, size_t max_length) {
	const char *name = "cradle";
	frigg::infoLogger() << "mlibc: Broken gethostname() called!" << frigg::endLog;
	strncpy(buffer, name, max_length);
	return 0;
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
pid_t getsid(pid_t) {
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
ssize_t readlink(const char *__restrict path, char *__restrict buffer, size_t max_size) {
	ssize_t length;
	if(mlibc::sys_readlink(path, buffer, max_size, &length))
		return -1;
	return length;
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
	frigg::infoLogger() << "\e[31mmlibc: seteuid() is a no-op\e[39m" << frigg::endLog;
	return 0;
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

char *ttyname(int fd) {
	const size_t size = 128;
	static thread_local char buf[size];
	if(mlibc::sys_ttyname(fd, buf, size))
		return nullptr;
	return buf;
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

char *optarg;
int optind;
int opterr;
int optopt;

int getpagesize(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

char *get_current_dir_name(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

// This is a Linux extension
pid_t gettid(void) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}


ssize_t write(int fd, const void *buf, size_t count) {
	ssize_t bytes_written;
	if(mlibc::sys_write(fd, buf, count, &bytes_written))
		return (ssize_t)-1;
	return bytes_written;
}

ssize_t read(int fd, void *buf, size_t count) {
	ssize_t bytes_read;
	if(mlibc::sys_read(fd, buf, count, &bytes_read))
		return (ssize_t)-1;
	return bytes_read;
}

off_t lseek(int fd, off_t offset, int whence) {
	off_t new_offset;
	if(mlibc::sys_seek(fd, offset, whence, &new_offset))
		return (off_t)-1;
	return new_offset;
}

int close(int fd) {
	return mlibc::sys_close(fd);
}

unsigned int sleep(unsigned int secs) {
	time_t seconds = secs;
	long nanos = 0;
	mlibc::sys_sleep(&seconds, &nanos);
	return seconds;
}

// In contrast to sleep() this functions returns 0/-1 on success/failure.
int usleep(useconds_t usecs) {
	time_t seconds = 0;
	long nanos = usecs * 1000;
	return mlibc::sys_sleep(&seconds, &nanos); 
}

int dup(int fd) {
	int newfd;
	if(mlibc::sys_dup(fd, &newfd))
		return -1;
	return newfd;
}

int dup2(int fd, int newfd) {
	if(mlibc::sys_dup2(fd, newfd))
		return -1;
	return newfd;
}

pid_t fork(void) {
	pid_t child;
	if(mlibc::sys_fork(&child))
		return -1;
	return child;
}

int execve(const char *path, char *const argv[], char *const envp[]) {
	mlibc::sys_execve(path, argv, envp);
	return -1;
}

gid_t getgid(void) {
	return mlibc::sys_getgid();
}
gid_t getegid(void) {
	return mlibc::sys_getegid();
}

uid_t getuid(void) {
	return mlibc::sys_getuid();
}
uid_t geteuid(void) {
	return mlibc::sys_geteuid();
}

pid_t getpid(void) {
	return mlibc::sys_getpid();
}

pid_t getppid(void) {
	return mlibc::sys_getppid();
}

int access(const char *path, int mode) {
	return mlibc::sys_access(path, mode);
}

int isatty(int fd) {
	int val;
	if(mlibc::sys_isatty(fd, &val)) {
		return 0;
	}
	return val;
}

int pipe2(int *pipefd, int flags) {
	__ensure(!"Not implemented");
	__builtin_unreachable();
}

int chroot(const char *ptr) {
	if(mlibc::sys_chroot(ptr))
		return -1;
	return 0;
}


