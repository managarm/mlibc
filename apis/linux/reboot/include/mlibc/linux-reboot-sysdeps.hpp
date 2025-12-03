#ifndef MLIBC_APIS_LINUX_REBOOT_SYSDEPS
#define MLIBC_APIS_LINUX_REBOOT_SYSDEPS

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_reboot(int cmd);

} // namespace mlibc

#endif // MLIBX_APIS_LINUX_REBOOT_SYSDEPS
