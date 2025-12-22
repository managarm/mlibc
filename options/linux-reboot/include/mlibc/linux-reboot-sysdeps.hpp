#ifndef MLIBC_LINUX_REBOOT_SYSDEPS
#define MLIBC_LINUX_REBOOT_SYSDEPS

#include <sys/reboot.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_reboot(int cmd);

} // namespace mlibc

#endif // MLIBC_LINUX_REBOOT_SYSDEPS
