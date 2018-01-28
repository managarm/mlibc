
#include <time.h>
#include <bits/posix/ssize_t.h>
#include <bits/posix/off_t.h>

namespace mlibc {

__attribute__ ((noreturn)) void sys_exit(int status);
int sys_clock_get(time_t *secs);
int sys_open(const char *pathname, int flags, int *fd);
int sys_read(int fd, void *buf, size_t count, ssize_t *bytes_read);
int sys_write(int fd, const void *buf, size_t count, ssize_t *bytes_written);
int sys_seek(int fd, off_t offset, int whence, off_t *new_offset);

} //namespace mlibc

