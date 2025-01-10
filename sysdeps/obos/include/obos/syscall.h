#pragma once

#include <stdint.h>

enum {
    Sys_ExitCurrentThread,
    Sys_Yield,
    Sys_Reboot,
    Sys_Shutdown,
    Sys_HandleClose,
    Sys_HandleClone,
    Sys_ThreadContextCreate, // 6
    Sys_Suspend,
    Sys_ThreadOpen,
    Sys_ThreadCreate,
    Sys_ThreadReady,
    Sys_ThreadBlock,
    Sys_ThreadBoostPriority,
    Sys_ThreadPriority,
    Sys_ThreadAffinity,
    Sys_ThreadSetOwner,
    Sys_ThreadGetTid, // 16
    Sys_WaitOnObject,
    Sys_WaitOnObjects,
    Sys_ProcessOpen,  // Unimplemented
    Sys_ProcessStart,
    Sys_KillProcess, // signal-related
    Sys_VirtualMemoryAlloc, // 22
    Sys_VirtualMemoryFree,
    Sys_VirtualMemoryProtect,
    Sys_VirtualMemoryLock, // Unimplemented
    Sys_VirtualMemoryUnlock, // Unimplemented
    Sys_MakeNewContext,
    Sys_ContextExpandWSCapacity,
    Sys_ContextGetStat,
    Sys_GetUsedPhysicalMemoryCount,
    Sys_QueryPageInfo,
    Sys_FutexWake, // 32
    Sys_FutexWait,
    Sys_FdAlloc, // 34
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
    Sys_FdFlush, // 45
    Sys_PartProbeAllDrives,
    Sys_PartProbeDrive, // 47
    Sys_SigReturn,
    Sys_Kill,
    Sys_SigAction,
    Sys_SigSuspend,
    Sys_SigProcMask,
    Sys_SigAltStack, // 53
    Sys_OpenDir,
    Sys_ReadEntries, // 55
    Sys_ExecVE, // 56
    Sys_LibCLog, // 57
    Sys_ProcessGetPID, // 58
    Sys_ProcessGetPPID, // 59
    Sys_FdOpenAt,
    Sys_MmFork, // 61
    Sys_ExitCurrentProcess, // 62
    Sys_ProcessGetStatus,
    Sys_WaitProcess, // 64
    Sys_Stat,
    Sys_StatFSInfo,
    Sys_SysConf,
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
