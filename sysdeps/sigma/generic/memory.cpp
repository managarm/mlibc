#include <string.h>

#include <bits/ensure.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <libsigma/sys.h>
#include <errno.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc
{
    int sys_anon_allocate(size_t size, void **pointer){
        __ensure((size & 0xFFF) == 0);

        void* ret = libsigma_vm_map(size, NULL, NULL, PROT_READ | PROT_WRITE, MAP_ANONYMOUS);
        if(!ret)
            return ENOMEM;

        *pointer = ret;
        return 0;
    }
    int sys_anon_free(void *pointer, size_t size){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
        return 0;
    }
} // namespace mlibc
