
#ifndef _SETJMP_H
#define _SETJMP_H

#include <mlibc-config.h>
#include <bits/machine.h>
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// [C11/7.13] Non-local jumps

typedef struct __jmp_buf {
	struct __mlibc_jmpbuf_register_state reg_state;
} jmp_buf[1];

#ifndef __MLIBC_ABI_ONLY

__attribute__((__returns_twice__)) int setjmp(jmp_buf buffer);
__attribute__((__noreturn__)) void longjmp(jmp_buf buffer, int value);

#endif /* !__MLIBC_ABI_ONLY */

// POSIX Non-local jumps signal extensions

typedef struct {
	struct __mlibc_jmpbuf_register_state reg_state;
	int savesigs;
	sigset_t sigset;
} sigjmp_buf[1];

#ifndef __MLIBC_ABI_ONLY

#if __MLIBC_POSIX_OPTION
__attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf buffer, int savesigs);
__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf buffer, int value);
#endif // __MLIBC_POSIX_OPTION

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif // _SETJMP_H

