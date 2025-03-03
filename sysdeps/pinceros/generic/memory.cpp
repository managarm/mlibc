
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{

    int sys_anon_allocate(size_t /* size */, void **/* pointer */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

    int sys_anon_free(void */* pointer */, size_t /* size */)
    {
        MLIBC_UNIMPLEMENTED();
        return -1;
    }

} // namespace mlibc
