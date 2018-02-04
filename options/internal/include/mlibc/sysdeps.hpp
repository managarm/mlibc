
#include <time.h>
#include <bits/posix/ssize_t.h>
#include <bits/posix/off_t.h>
#include <bits/posix/pid_t.h>
#include <bits/posix/stat.h>

namespace mlibc {

__attribute__ ((noreturn)) void sys_exit(int status);

int sys_clock_get(time_t *secs);

int sys_open(const char *pathname, int flags, int *fd);
int sys_open_dir(const char *path, int *handle);
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
int sys_read_entries(int handle, void *buffer, size_t max_size, size_t *bytes_read);
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);
int sys_access(const char *path, int mode);
int sys_close(int fd);
int sys_dup2(int fd, int newfd);
int sys_isatty(int fd, int *ptr);

int sys_stat(const char *pathname, struct stat *statbuf);
int sys_fstat(int fd, struct stat *statbuf);
int sys_readlink(const char *path, void *buffer, size_t max_size, ssize_t *length);

gid_t sys_getgid();
gid_t sys_getegid();
uid_t sys_getuid();
uid_t sys_geteuid();
pid_t sys_getpid();
pid_t sys_getppid();

int sys_anon_allocate(size_t size, void **pointer);
int sys_anon_free(void *pointer, size_t size);

void sys_yield();

int sys_sleep(time_t *secs, long *nanos);

int sys_fork(pid_t *child);
void sys_execve(const char *path, char *const argv[], char *const envp[]);

} //namespace mlibc

