#include <string.h>

#include <bits/ensure.h>
#include <mlibc/sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <libsigma/memory.h>
#include <errno.h>

#define STUB_ONLY { __ensure(!"STUB_ONLY function was called"); __builtin_unreachable(); }

namespace mlibc
{
    int sys_anon_allocate(size_t size, void **pointer){
        __ensure((size & 0xFFF) == 0);
        uint64_t ret = libsigma_valloc(LIBSIGMA_VALLOC_TYPE_SBRK_LIKE, 0, size / 0x1000);
        if(ret == 1) return ENOMEM;
        *pointer = (void*)ret;

        return 0;
    }
    int sys_anon_free(void *pointer, size_t size){
        mlibc::infoLogger() << "mlibc: " << __func__ << " is a stub!" << frg::endlog;
        return 0;
    }
} // namespace mlibc
