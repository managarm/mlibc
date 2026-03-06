#ifndef MLIBC_LINUX_WRAPPER_SYSDEPS
#define MLIBC_LINUX_WRAPPER_SYSDEPS

#include <abi-bits/pid_t.h>
#include <abi-bits/signal.h>

namespace [[gnu::visibility("hidden")]] mlibc {

[[gnu::weak]] int sys_rt_sigqueueinfo(pid_t tgid, int sig, siginfo_t *uinfo);
[[gnu::weak]] int sys_rt_tgsigqueueinfo(pid_t tgid, pid_t tid, int sig, siginfo_t *uinfo);

} // namespace mlibc

#endif // MLIBC_LINUX_WRAPPER_SYSDEPS
