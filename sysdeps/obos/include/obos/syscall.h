#pragma once

#include <stdint.h>

enum {
    Sys_ExitCurrentThread,
    Sys_Yield,
    Sys_Reboot,
    Sys_Shutdown,
    Sys_HandleClose,
    Sys_HandleClone,
    Sys_ThreadContextCreate,
    Sys_Suspend,
    Sys_ThreadOpen,
    Sys_ThreadCreate,
    Sys_ThreadReady,
    Sys_ThreadBlock,
    Sys_ThreadBoostPriority,
    Sys_ThreadPriority,
    Sys_ThreadAffinity,
    Sys_ThreadSetOwner,
    Sys_ThreadGetTid,
    Sys_WaitOnObject,
    Sys_WaitOnObjects,
    Sys_ProcessOpen,
    Sys_ProcessStart,
    Sys_KillProcess,
    Sys_VirtualMemoryAlloc,
    Sys_VirtualMemoryFree,
    Sys_VirtualMemoryProtect,
    Sys_VirtualMemoryLock,
    Sys_VirtualMemoryUnlock,
    Sys_MakeNewContext,
    Sys_ContextExpandWSCapacity,
    Sys_ContextGetStat,
    Sys_GetUsedPhysicalMemoryCount,
    Sys_QueryPageInfo,
    Sys_FutexWake,
    Sys_FutexWait,
    Sys_FdAlloc,
    Sys_FdOpen,
    Sys_FdOpenDirent,
    Sys_FdWrite,
    Sys_FdAWrite,
    Sys_FdRead,
    Sys_FdARead,
    Sys_FdSeek,
    Sys_FdTellOff,
    Sys_FdEOF,
    Sys_FdIoctl,
    Sys_FdFlush,
    Sys_PartProbeAllDrives,
    Sys_PartProbeDrive,
    Sys_SigReturn,
    Sys_Kill,
    Sys_SigAction,
    Sys_SigSuspend,
    Sys_SigProcMask,
    Sys_SigAltStack,
    Sys_OpenDir,
    Sys_ReadEntries,
    Sys_ExecVE,
    Sys_LibCLog,
    Sys_ProcessGetPID,
    Sys_ProcessGetPPID,
    Sys_FdOpenAt,
    Sys_MmFork,
    Sys_ExitCurrentProcess,
    Sys_ProcessGetStatus,
    Sys_WaitProcess,
    Sys_Stat,
    Sys_StatFSInfo,
    Sys_SysConf,
    Sys_SetKLogLevel,
    Sys_LoadDriver,
    Sys_StartDriver,
    Sys_UnloadDriver,
    Sys_PnpLoadDriversAt,
    Sys_FindDriverByName,
    Sys_EnumerateLoadedDrivers,
    Sys_QueryDriverName,
    Sys_Sync,
    Sys_SleepMS,
};

#ifdef __cplusplus
extern "C" {
#endif
    uintptr_t syscall(uint32_t num, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4);
#ifdef __cplusplus
}
#endif

#define syscall0(num) syscall(num, 0,0,0,0,0)
#define syscall1(num, arg0) syscall(num, (uintptr_t)(arg0),0,0,0,0)
#define syscall2(num, arg0, arg1) syscall(num, (uintptr_t)(arg0),(uintptr_t)(arg1), 0,0,0)
#define syscall3(num, arg0, arg1, arg2) syscall(num, (uintptr_t)(arg0),(uintptr_t)(arg1),(uintptr_t)(arg2), 0,0)
#define syscall4(num, arg0, arg1, arg2, arg3) syscall(num, (uintptr_t)(arg0),(uintptr_t)(arg1),(uintptr_t)(arg2),(uintptr_t)(arg3), 0)
#define syscall5(num, arg0, arg1, arg2, arg3, arg4) syscall(num, (uintptr_t)(arg0),(uintptr_t)(arg1),(uintptr_t)(arg2),(uintptr_t)(arg3),(uintptr_t)(arg4))

#define HANDLE_VALUE_MASK (0xffffff)
#define HANDLE_TYPE_SHIFT (24UL)

typedef uint8_t handle_type;

#define HANDLE_TYPE(hnd) (handle_type)((hnd) >> HANDLE_TYPE_SHIFT)
#define HANDLE_VALUE(hnd) (unsigned int)((hnd) & HANDLE_VALUE_MASK)
typedef uint32_t handle;
#define HANDLE_INVALID (handle)((handle)0xff << HANDLE_TYPE_SHIFT)
#define HANDLE_CURRENT (handle)((handle)0xfe << HANDLE_TYPE_SHIFT)
#define HANDLE_ANY     (handle)((handle)0xfd     << HANDLE_TYPE_SHIFT)
