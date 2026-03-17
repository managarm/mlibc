#include <errno.h>
#include <menix/syscall.hpp>
#include <mlibc/all-sysdeps.hpp>

#define MENIX_REBOOT_MAGIC 0xDEADBEEF

namespace mlibc {

int Sysdeps<Reboot>::operator()(int cmd) { return menix_syscall(SYSCALL_REBOOT, MENIX_REBOOT_MAGIC, cmd).error; }

} // namespace mlibc
