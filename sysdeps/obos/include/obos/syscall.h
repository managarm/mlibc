#ifndef __OBOS_SYSCALL_H
#define __OBOS_SYSCALL_H

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
	Sys_Fcntl,
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
	Sys_IRPCreate,
	Sys_FdRead,
	Sys_IRPSubmit,
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
	Sys_Mount,
	Sys_Unmount,
	Sys_FdCreat,
	Sys_FdOpenEx,
	Sys_FdOpenAtEx,
	Sys_Mkdir,
	Sys_MkdirAt,
	Sys_Chdir,
	Sys_ChdirEnt,
	Sys_GetCWD,
	Sys_SetControllingTTY,
	Sys_GetControllingTTY,
	Sys_TTYName,
	Sys_IsATTY,
	Sys_IRPWait,
	Sys_IRPQueryState,
	Sys_IRPGetBuffer,
	Sys_IRPGetStatus,
	Sys_CreatePipe,
	Sys_PSelect,
	Sys_ReadLinkAt,
	Sys_SetUid,
	Sys_SetGid,
	Sys_GetUid,
	Sys_GetGid,
	Sys_UnlinkAt,
	Sys_MakeDiskSwap,
	Sys_SwitchSwap,
	Sys_SyncAnonPages,
	Sys_FdPWrite,
	Sys_FdPRead,
	Sys_SymLink,
	Sys_SymLinkAt,
	Sys_CreateNamedPipe,
	Sys_PPoll,
	Sys_Socket,
	Sys_SendTo,
	Sys_RecvFrom,
	Sys_Listen,
	Sys_Accept,
	Sys_Bind,
	Sys_Connect,
	Sys_SockName,
	Sys_PeerName,
	Sys_GetSockOpt,
	Sys_SetSockOpt,
	Sys_ShutdownSocket,
	Sys_GetHostname,
	Sys_SetHostname,
	Sys_KillProcessGroup,
	Sys_SetProcessGroup,
	Sys_GetProcessGroup,
	Sys_LinkAt,
	Sys_FChmodAt,
	Sys_FChownAt,
	Sys_UMask,
	Sys_RenameAt,
	Sys_UTimeNSAt,
	Sys_ThreadGetStack,
	Sys_SetRESUid,
	Sys_SetRESGid,
	Sys_GetRESUid,
	Sys_GetRESGid,
	Sys_SigPending,
	Sys_SetGroups,
	Sys_GetGroups,
	Sys_GetCachedByteCount,
	Sys_GetHDADevices,
	Sys_SetSid,
	Sys_GetSid,
};

#if defined(__x86_64__)
enum {
	SysS_SetFSBase = 0x80000000,
	SysS_ThreadContextCreateFork,
	SysS_ClockGet,
	SysS_GDBStubBindInet,
	SysS_GDBStubBindDevice,
	SysS_GDBStubStart,
	SysS_QueryTSCFrequency,
};
#elif defined(__m68k__)
enum {
	SysS_SetTCB = 0x80000000,
	SysS_GetTCB,
	SysS_ThreadContextCreateFork,
};
#endif

#ifdef __cplusplus
extern "C" {
#endif
uintptr_t syscall(
    uint32_t num, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3, uintptr_t arg4
);
#ifdef __cplusplus
}
#endif

#define syscall0(num) syscall(num, 0, 0, 0, 0, 0)
#define syscall1(num, arg0) syscall(num, (uintptr_t)(arg0), 0, 0, 0, 0)
#define syscall2(num, arg0, arg1) syscall(num, (uintptr_t)(arg0), (uintptr_t)(arg1), 0, 0, 0)
#define syscall3(num, arg0, arg1, arg2)                                                            \
	syscall(num, (uintptr_t)(arg0), (uintptr_t)(arg1), (uintptr_t)(arg2), 0, 0)
#define syscall4(num, arg0, arg1, arg2, arg3)                                                      \
	syscall(num, (uintptr_t)(arg0), (uintptr_t)(arg1), (uintptr_t)(arg2), (uintptr_t)(arg3), 0)
#define syscall5(num, arg0, arg1, arg2, arg3, arg4)                                                \
	syscall(                                                                                       \
	    num,                                                                                       \
	    (uintptr_t)(arg0),                                                                         \
	    (uintptr_t)(arg1),                                                                         \
	    (uintptr_t)(arg2),                                                                         \
	    (uintptr_t)(arg3),                                                                         \
	    (uintptr_t)(arg4)                                                                          \
	)

#define HANDLE_VALUE_MASK (0xffffff)
#define HANDLE_TYPE_SHIFT (24UL)

typedef uint8_t handle_type;

#define HANDLE_TYPE(hnd) (handle_type)((hnd) >> HANDLE_TYPE_SHIFT)
#define HANDLE_VALUE(hnd) (unsigned int)((hnd) & HANDLE_VALUE_MASK)
typedef uint32_t handle;
#define HANDLE_INVALID (handle)((handle)0xff << HANDLE_TYPE_SHIFT)
#define HANDLE_CURRENT (handle)((handle)0xfe << HANDLE_TYPE_SHIFT)
#define HANDLE_ANY (handle)((handle)0xfd << HANDLE_TYPE_SHIFT)

#endif /* __OBOS_SYSCALL_H */
