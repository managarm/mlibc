#ifndef _SYS_SYSCALL_H
#define _SYS_SYSCALL_H

// OpenSSL wants this header to exist
#ifdef __linux__
#include <abi-bits/arch-syscall.h>
#include <asm/unistd.h>
#endif

#endif // _SYS_SYSCALL_H
