#include <mlibc/all-sysdeps.hpp>

#include <piggy/syscall.h>

#include <string.h>

namespace mlibc {
    int Sysdeps<GetHostname>::operator()(char* buffer, size_t bufsize) {
        struct utsname utsname;

        int ret = sysdep<Uname>(&utsname);
        if (ret != 0) {
            return ret;
        }

        if (bufsize >= sizeof(utsname.nodename)) {
            bufsize = sizeof(utsname.nodename) - 1;
        }

        memcpy(buffer, utsname.nodename, bufsize);
        return 0;
    }

    int Sysdeps<SetHostname>::operator()(const char* buffer, size_t bufsize) {
        long ret = syscall2(SYS_SETHOSTNAME, (long) buffer, bufsize);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    int Sysdeps<Uname>::operator()(struct utsname* buf) {
        long ret = syscall1(SYS_UNAME, (long) buf);
        if (ret < 0) {
            return -ret;
        }

        return 0;
    }

    // stubs only below this point

    int Sysdeps<Access>::operator()(const char* path, int mode) {
        (void) path;
        (void) mode;
        return 0;
    }

    uid_t Sysdeps<GetUid>::operator()(void) {
        return 0;
    }

    uid_t Sysdeps<GetEuid>::operator()(void) {
        return 0;
    }

    gid_t Sysdeps<GetGid>::operator()(void) {
        return 0;
    }

    gid_t Sysdeps<GetEgid>::operator()(void) {
        return 0;
    }

    int Sysdeps<GetResuid>::operator()(uid_t* ruid, uid_t* euid, uid_t* suid) {
        *ruid = *euid = *suid = 0;
        return 0;
    }

    int Sysdeps<GetResgid>::operator()(gid_t* rgid, gid_t* egid, gid_t* sgid) {
        *rgid = *egid = *sgid = 0;
        return 0;
    }
} // namespace mlibc
