
#ifndef _SETJMP_H
#define _SETJMP_H

#include <bits/machine.h>
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

// [C11/7.13] Non-local jumps

// FIXME: use intptr_t equivalent
typedef char jmp_buf[__MLIBC_JMPBUF_SIZE * sizeof(void *)];
typedef char sigjmp_buf[sizeof(sigset_t) + (__MLIBC_JMPBUF_SIZE * sizeof(void *))];

__attribute__ (( returns_twice )) int setjmp(jmp_buf buffer);
__attribute__ (( noreturn )) void longjmp(jmp_buf buffer, int value);
__attribute__ (( returns_twice )) int sigsetjmp(sigjmp_buf buffer, int savesigs);
__attribute__ (( noreturn )) void siglongjmp(sigjmp_buf buffer, int value);

#ifdef __cplusplus
}
#endif

#endif // _SETJMP_H

