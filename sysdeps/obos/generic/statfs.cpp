#include <errno.h>

extern "C" {

int statfs(const char *path, struct statfs *buf)
{
	errno = ENOSYS;
    return -1;
}

int fstatfs(int fd, struct statfs *buf)
{
	errno = ENOSYS;
    return -1;
}

[[gnu::alias("fstatfs")]] int fstatfs64(int, struct statfs64 *);

}
