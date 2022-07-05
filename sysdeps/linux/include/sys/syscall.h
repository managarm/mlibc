#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

#include <abi-bits/arch-syscall.h>
#if __has_include(<asm/unistd.h>)
#include <asm/unistd.h>
#endif

#endif // _SYS_SYSCALL_H
