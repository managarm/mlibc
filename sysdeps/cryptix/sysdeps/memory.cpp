#include <cstddef>
#include <stdio.h>
#include <stdlib.h>

#include <cryptix/syscall.h>

#include <execinfo.h>
#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <sys/mman.h>

namespace mlibc
{
    int sys_anon_allocate(size_t size, void** pointer)
    {
        auto ret = sys_vm_map(nullptr, size, PROT_READ | PROT_WRITE, 0x20, -1,
                              0, pointer);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_anon_free(void* pointer, size_t size)
    {
        auto ret = sys_vm_unmap(pointer, size);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }

    int sys_vm_map(void* hint, size_t size, int prot, int flags, int fd,
                   off_t offset, void** window)
    {
        auto ret = Syscall(SYS_MMAP, hint, size, prot, flags, fd, offset);
        if (auto e = syscall_error(ret); e) return e;

        *window = reinterpret_cast<void*>(ret);
        return 0;
    }
    int sys_vm_unmap(void* pointer, size_t size)
    {
        auto ret = Syscall(SYS_MUNMAP, pointer, size);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_vm_protect(void* pointer, size_t size, int prot)
    {
        auto ret = Syscall(SYS_MPROTECT, pointer, size, prot);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
} // namespace mlibc
