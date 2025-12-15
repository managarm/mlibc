#ifndef MLIBC_LINUX_TIMERFD_SYSDEPS
#define MLIBC_LINUX_TIMERFD_SYSDEPS

#include <sys/timerfd.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_timerfd_create(int clockid, int flags, int *fd);
[[gnu::weak]] int sys_timerfd_settime(int fd, int flags,
		const struct itimerspec *value, struct itimerspec *oldvalue);
[[gnu::weak]] int sys_timerfd_gettime(int fd, struct itimerspec *its);

} // namespace mlibc

#endif // MLIBC_LINUX_TIMERFD_SYSDEPS
