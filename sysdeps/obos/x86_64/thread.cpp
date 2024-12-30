#include <mlibc/internal-sysdeps.hpp>

#include <obos/syscall.h>

namespace [[gnu::visibility("hidden")]] mlibc {

int sys_tcb_set(void *pointer)
{
    syscall1(0x80000000 /* Sys_SetFSBase */, pointer);
    return 0;
}

}
