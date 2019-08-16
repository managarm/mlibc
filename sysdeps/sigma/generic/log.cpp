#include <string.h>
#include <assert.h>

#include <bits/ensure.h>
#include <mlibc/debug.hpp>
#include <mlibc/sysdeps.hpp>

#include <libsigma/klog.h>
#include <libsigma/thread.h>

#define UNIMPLEMENTED() { __ensure(!"Unimplemented function was called"); __builtin_unreachable(); }

namespace mlibc
{
    void sys_libc_log(const char *message) {
        libsigma_klog(message);
    }

    void sys_libc_panic(){
        mlibc::infoLogger() << "mlibc: panic!" << frg::endlog;
        libsigma_kill();
    }
} // namespace mlibc
