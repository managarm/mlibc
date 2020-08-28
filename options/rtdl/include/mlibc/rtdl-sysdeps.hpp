#ifndef MLIBC_RTDL_SYSDEPS
#define MLIBC_RTDL_SYSDEPS

namespace mlibc [[gnu::visibility("hidden")]] {

int sys_tcb_set(void *pointer);

} // namespace mlibc

#endif // MLIBC_RTDL_SYSDEPS
