#include <mlibc/debug.hpp>
#include <mlibc/posix-sysdeps.hpp>

#include <cryptix/syscall.h>

#include <bits/ensure.h>
#include <cryptix/reboot.hpp>
#include <errno.h>
#include <sys/reboot.h>

using namespace cryptix;

RebootCmd cryptix_reboot_cmd(unsigned int what)
{
    switch (what)
    {
        case 0x01234567: return RebootCmd::eRestart;
        case 0xCDEF0123: return RebootCmd::eHalt;
        case 0x4321FEDC: return RebootCmd::ePowerOff;
        case 0xA1B2C3D4: return RebootCmd::eRestart2;
        case 0xD000FCE2: return RebootCmd::eSuspend;
        case 0x45584543: return RebootCmd::eKexec;

        default: break;
    }

    return RebootCmd::eUndefined;
}

namespace mlibc
{
    int sys_getrlimit(int resource, struct rlimit* rlim)
    {
        auto ret = Syscall(SYS_GETRLIMIT, resource, rlim);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_getrusage(int who, struct rusage* usage)
    {
        auto ret = Syscall(SYS_GETRUSAGE, who, usage);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
    int sys_reboot(int what)
    {
        auto cmd = cryptix_reboot_cmd(what);

        auto ret = Syscall(SYS_REBOOT, cmd);
        if (auto e = syscall_error(ret); e) return e;

        return 0;
    }
}; // namespace mlibc
