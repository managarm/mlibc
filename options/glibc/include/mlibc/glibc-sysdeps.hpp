#ifndef MLIBC_GLIBC_SYSDEPS
#define MLIBC_GLIBC_SYSDEPS

namespace  [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_ioctl(int fd, unsigned long request, void *arg, int *result);
[[gnu::weak]] int sys_tgkill(int tgid, int tid, int sig);

[[gnu::weak]] int sys_personality(unsigned long persona, int *out);

[[gnu::weak]] int sys_ioperm(unsigned long int from, unsigned long int num, int turn_on);
[[gnu::weak]] int sys_iopl(int level);

} // namespace mlibc

#endif // MLIBC_GLIBC_SYSDEPS
