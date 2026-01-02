#include <errno.h>
#include <menix/syscall.hpp>
#include <mlibc/linux-reboot-sysdeps.hpp>

#define MENIX_REBOOT_MAGIC 0xDEADBEEF

namespace mlibc {

int sys_reboot(int cmd) { return menix_syscall(SYSCALL_REBOOT, MENIX_REBOOT_MAGIC, cmd).error; }

} // namespace mlibc
