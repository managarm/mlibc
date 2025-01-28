#ifndef _KEYRONEX__SYSCALL_H
#define _KEYRONEX__SYSCALL_H

#include <stdint.h>

enum posix_syscall {
	/*! debug print */
	kPXSysDebug,
	kPXSysMmap,
	kPXSysMunmap,

	kPXSysIOCtl,
	kPXSysOpenAt,
	kPXSysClose,
	kPXSysRead,
	kPXSysReadLink,
	kPXSysWrite,
	kPXSysSeek,
	kPXSysPPoll,
	kPXSysIsATTY,
	kPXSysReadDir,
	kPXSysStat,
	kPXSysUnlinkAt,
	kPXSysGetCWD,
	kPXSysPipe,
	kPXSysDup,
	kPXSysDup3,
	kPXSysLink,
	kPXSysChDir,
	kPXSysUMask,
	kPXSysMkDirAt,
	kPXSysRenameAt,
	kPXSysStatFS,
	kPXSysFCntl,

	kPXSysSetFSBase,
	kPXSysExecVE,
	kPXSysExit,
	kPXSysFork,
	kPXSysWaitPID,
	kPXSysGetPID,
	kPXSysGetPPID,
	kPXSysGetPGID,
	kPXSysSetPGID,
	kPXSysGetSID,
	kPXSysSetSID,
	kPXSysGetTID,

	kPXSysSigAction,
	kPXSysSigMask,
	kPXSysSigSend,
	kPXSysSigSuspend,
	kPXSysSigTimedWait,

	kPXSysSocket,
	kPXSysBind,
	kPXSysConnect,
	kPXSysListen,
	kPXSysAccept,
	kPXSysSendMsg,
	kPXSysRecvMsg,
	kPXSysSocketPair,
	kPXSysGetSockOpt,
	kPXSysSetSockOpt,

	kPXSysEPollCreate,
	kPXSysEPollCtl,
	kPXSysEPollWait,

	kPXSysSleep,
	kPXSysUTSName,
	kPXSysClockGet,

	kPXSysForkThread,
	kPXSysFutexWait,
	kPXSysFutexWake,

	/*! register signal entry function */
	kPXSysSigEntry,
	/*! return from a signal */
	kPXSysSigReturn,
};

enum posix_stat_kind {
	kPXStatKindFD,
	kPXStatKindAt,
	kPXStatKindCWD,
};

#if defined(__x86_64__)
static inline uintptr_t
syscall0(uintptr_t num, uintptr_t *out)
{
	uintptr_t ret, ret2;
	asm volatile("int $0x80" : "=a"(ret), "=D"(ret2) : "a"(num) : "memory");
	if (out)
		*out = ret2;
	return ret;
}

static inline uintptr_t
syscall1(uintptr_t num, uintptr_t arg1, uintptr_t *out)
{
	uintptr_t ret, ret2;
	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1)
		     : "memory");
	if (out)
		*out = ret2;
	return ret;
}

static inline uintptr_t
syscall2(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t *out)
{
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall3(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t *out)
{
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall4(intptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall5(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4, r8 asm("r8") = arg5;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
		     "r"(r8)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline uintptr_t
syscall6(uintptr_t num, uintptr_t arg1, uintptr_t arg2, uintptr_t arg3,
    uintptr_t arg4, uintptr_t arg5, uintptr_t arg6, uintptr_t *out)
{
	register uintptr_t r10 asm("r10") = arg4, r8 asm("r8") = arg5,
			       r9 asm("r9") = arg6;
	uintptr_t ret, ret2;

	asm volatile("int $0x80"
		     : "=a"(ret), "=D"(ret2)
		     : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10),
		     "r"(r8), "r"(r9)
		     : "memory");

	if (out)
		*out = ret2;

	return ret;
}

static inline int
sc_error(uintptr_t ret)
{
	if (ret > -4096UL)
		return -ret;
	return 0;
}
#endif

#endif
