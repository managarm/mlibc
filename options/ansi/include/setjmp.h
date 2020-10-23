
#ifndef _SETJMP_H
#define _SETJMP_H

#include <bits/feature.h>
#include <bits/machine.h>
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// [C11/7.13] Non-local jumps

typedef struct {
	struct __mlibc_jmpbuf_register_state reg_state;
} jmp_buf[1];

__attribute__ (( returns_twice )) int setjmp(jmp_buf buffer);
__attribute__ (( noreturn )) void longjmp(jmp_buf buffer, int value);

// POSIX Non-local jumps signal extensions

typedef struct {
	struct __mlibc_jmpbuf_register_state reg_state;
	int savesigs;
	sigset_t sigset;
} sigjmp_buf[1];

#if __MLIBC_POSIX_OPTION
__attribute__ (( returns_twice )) int sigsetjmp(sigjmp_buf buffer, int savesigs);
__attribute__ (( noreturn )) void siglongjmp(sigjmp_buf buffer, int value);
#endif // __MLIBC_POSIX_OPTION

#ifdef __cplusplus
}
#endif

#endif // _SETJMP_H

