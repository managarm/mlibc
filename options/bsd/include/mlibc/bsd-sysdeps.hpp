#ifndef MLIBC_BSD_SYSDEPS
#define MLIBC_BSD_SYSDEPS

#include <ifaddrs.h>
#include <termios.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_brk(void **out);

[[gnu::weak]] int sys_getloadavg(double *samples);

[[gnu::weak]] int sys_openpty(int *mfd, int *sfd, char *name, const struct termios *ios, const struct winsize *win);

[[gnu::weak]] int sys_getifaddrs(struct ifaddrs **);

} // namespace mlibc

#endif // MLIBC_BSD_SYSDEPS
