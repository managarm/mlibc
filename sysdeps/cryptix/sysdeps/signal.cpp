#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <cryptix/syscall.h>

namespace mlibc
{
    int sys_sigaction([[maybe_unused]] int signum,
                      [[maybe_unused]] const struct sigaction* __restrict act,
                      [[maybe_unused]] struct sigaction* __restrict oldact)
    {
        // TODO(v1tr10l7): implement sys_sigaction;
        return 0;
    }

    int sys_sigprocmask([[maybe_unused]] int             how,
                        [[maybe_unused]] const sigset_t* set,
                        [[maybe_unused]] sigset_t*       retrieve)
    {

        // TODO(v1tr10l7): implement sys_sigprocmask;
        return 0;
    }
}; // namespace mlibc
