#include <mlibc/all-sysdeps.hpp>
#include <zinnia/syscall.hpp>

#define ZINNIA_REBOOT_MAGIC 0xDEADBEEF

namespace mlibc {

int Sysdeps<Reboot>::operator()(int cmd) {
	return zinnia_syscall(SYSCALL_REBOOT, ZINNIA_REBOOT_MAGIC, cmd).error;
}

} // namespace mlibc
