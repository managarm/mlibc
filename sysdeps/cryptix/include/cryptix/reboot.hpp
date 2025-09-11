#pragma once

namespace cryptix
{
    enum class RebootCmd
    {
        eRestart   = 0,
        eHalt      = 1,
        ePowerOff  = 2,
        eRestart2  = 3,
        eSuspend   = 4,
        eKexec     = 5,
        eUndefined = -1,
    };
};
