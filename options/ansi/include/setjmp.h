
#ifndef _SETJMP_H
#define _SETJMP_H

#include <bits/machine.h>

#ifdef __cplusplus
extern "C" {
#endif

// [C11/7.13] Non-local jumps

// FIXME: use intptr_t equivalent
typedef long jmp_buf[__MLIBC_JMPBUF_SIZE];

__attribute__ (( returns_twice )) int setjmp(jmp_buf buffer);
__attribute__ (( noreturn )) void longjmp(jmp_buf buffer, int value);

#ifdef __cplusplus
}
#endif

#endif // _SETJMP_H

