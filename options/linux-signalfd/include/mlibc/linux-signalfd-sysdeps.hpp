#ifndef MLIBC_LINUX_SIGNALFD_SYSDEPS
#define MLIBC_LINUX_SIGNALFD_SYSDEPS

#include <sys/signalfd.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_signalfd_create(const sigset_t *, int flags, int *fd);

} // namespace mlibc

#endif // MLIBC_LINUX_SIGNALFD_SYSDEPS
