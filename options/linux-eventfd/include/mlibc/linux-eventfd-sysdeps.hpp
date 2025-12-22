#ifndef MLIBC_LINUX_EVENTFD_SYSDEPS
#define MLIBC_LINUX_EVENTFD_SYSDEPS

#include <sys/eventfd.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_eventfd_create(unsigned int initval, int flags, int *fd);

} // namespace mlibc

#endif // MLIBC_LINUX_EVENTFD_SYSDEPS
