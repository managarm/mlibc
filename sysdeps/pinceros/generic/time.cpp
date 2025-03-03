#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{

    int sys_clock_get(int /* clock */, time_t * /* secs */, long * /* nanos */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }
}