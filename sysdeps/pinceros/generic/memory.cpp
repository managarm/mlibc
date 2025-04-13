
#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>

namespace mlibc
{

    int sys_anon_allocate(size_t size, void **pointer)
    {
        return sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                          -1, 0, pointer);
    }

    int sys_anon_free(void *pointer, size_t size)
    {
        return sys_vm_unmap(pointer, size);
    }

} // namespace mlibc
