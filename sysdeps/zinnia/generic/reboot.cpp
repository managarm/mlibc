#include <errno.h>
#include <zinnia/syscall.hpp>
#include <mlibc/all-sysdeps.hpp>

#define ZINNIA_REBOOT_MAGIC 0xDEADBEEF

namespace mlibc {

int Sysdeps<Reboot>::operator()(int cmd) { return zinnia_syscall(SYSCALL_REBOOT, ZINNIA_REBOOT_MAGIC, cmd).error; }

} // namespace mlibc
