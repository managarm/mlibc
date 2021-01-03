#ifndef MLIBC_GLIBC_SYSDEPS
#define MLIBC_GLIBC_SYSDEPS

namespace  [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result);

} // namespace mlibc

#endif // MLIBC_GLIBC_SYSDEPS
