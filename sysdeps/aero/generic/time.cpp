#include <mlibc/all-sysdeps.hpp>
#include <aero/syscall.h>

namespace mlibc {
int sys_setitimer(int which, const struct itimerval *new_value,
                  struct itimerval *old_value) {
    auto result = syscall(SYS_SETITIMER, which, new_value, old_value);

    if (result < 0) {
        return -result;
    }

    return 0;
}

int sys_getitimer(int which, struct itimerval *curr_value) {
    auto result = syscall(SYS_GETITIMER, which, curr_value);

    if (result < 0) {
        return -result;
    }

    return 0;
}
} // namespace mlibc