
#ifndef _SETJMP_H
#define _SETJMP_H

#include <mlibc-config.h>
#include <bits/machine.h>
#include <abi-bits/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

/* [C11/7.13] Non-local jumps */

typedef struct __jmp_buf {
	struct __mlibc_jmpbuf_register_state __reg_state;
} jmp_buf[1];

#ifndef __MLIBC_ABI_ONLY

__attribute__((__returns_twice__)) int setjmp(jmp_buf __buffer);
__attribute__((__noreturn__)) void longjmp(jmp_buf __buffer, int __value);

/* setjmp is defined as a function macro in the ISO C standard */
#define setjmp(env) setjmp(env)

#if __MLIBC_POSIX_OPTION
__attribute__((__returns_twice__)) int _setjmp(jmp_buf __buffer);
/* POSIX-2017.1 says _longjmp shall be declared as a function */
__attribute__((__noreturn__)) void _longjmp(jmp_buf __buffer, int __value);
#endif /* __MLIBC_POSIX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

/* POSIX Non-local jumps signal extensions */

typedef struct __sigjmp_buf {
	struct __mlibc_jmpbuf_register_state __reg_state;
	int __savesigs;
	sigset_t __sigset;
} sigjmp_buf[1];

#ifndef __MLIBC_ABI_ONLY

#if __MLIBC_POSIX_OPTION
__attribute__((__returns_twice__)) int sigsetjmp(sigjmp_buf __buffer, int __savesigs);
__attribute__((__noreturn__)) void siglongjmp(sigjmp_buf __buffer, int __value);
#endif /* __MLIBC_POSIX_OPTION */

#endif /* !__MLIBC_ABI_ONLY */

#ifdef __cplusplus
}
#endif

#endif /* _SETJMP_H */

