#ifndef MLIBC_BSD_SYSDEPS
#define MLIBC_BSD_SYSDEPS

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_brk(void **out);

[[gnu::weak]] int sys_getloadavg(double *samples);

} // namespace mlibc

#endif // MLIBC_BSD_SYSDEPS
