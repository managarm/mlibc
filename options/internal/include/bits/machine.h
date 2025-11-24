
#ifndef _MLIBC_INTERNAL_MACHINE_H
#define _MLIBC_INTERNAL_MACHINE_H

#include <bits/types.h>

#if defined (__i386__)
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint32 __ebx;
	__mlibc_uint32 __ebp;
	__mlibc_uint32 __esi;
	__mlibc_uint32 __edi;
	__mlibc_uint32 __esp;
	__mlibc_uint32 __eip;
};
#elif defined (__x86_64__)
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint64 __rbx;
	__mlibc_uint64 __rbp;
	__mlibc_uint64 __r12;
	__mlibc_uint64 __r13;
	__mlibc_uint64 __r14;
	__mlibc_uint64 __r15;
	__mlibc_uint64 __rsp;
	__mlibc_uint64 __rip;
};
#elif defined (__aarch64__)
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint64 __x19;
	__mlibc_uint64 __x20;
	__mlibc_uint64 __x21;
	__mlibc_uint64 __x22;
	__mlibc_uint64 __x23;
	__mlibc_uint64 __x24;
	__mlibc_uint64 __x25;
	__mlibc_uint64 __x26;
	__mlibc_uint64 __x27;
	__mlibc_uint64 __x28;
	__mlibc_uint64 __x29;
	__mlibc_uint64 __x30;
	__mlibc_uint64 __sp;
	__mlibc_uint64 __pad;
	__mlibc_uint64 __d8;
	__mlibc_uint64 __d9;
	__mlibc_uint64 __d10;
	__mlibc_uint64 __d11;
	__mlibc_uint64 __d12;
	__mlibc_uint64 __d13;
	__mlibc_uint64 __d14;
	__mlibc_uint64 __d15;
};
#elif defined (__riscv) && __riscv_xlen == 64
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint64 __ra;
	__mlibc_uint64 __s0;
	__mlibc_uint64 __s1;
	__mlibc_uint64 __s2;
	__mlibc_uint64 __s3;
	__mlibc_uint64 __s4;
	__mlibc_uint64 __s5;
	__mlibc_uint64 __s6;
	__mlibc_uint64 __s7;
	__mlibc_uint64 __s8;
	__mlibc_uint64 __s9;
	__mlibc_uint64 __s10;
	__mlibc_uint64 __s11;
	__mlibc_uint64 __sp;
	double __fs0;
	double __fs1;
	double __fs2;
	double __fs3;
	double __fs4;
	double __fs5;
	double __fs6;
	double __fs7;
	double __fs8;
	double __fs9;
	double __fs10;
	double __fs11;
};
#elif defined (__m68k__)
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint32 __d2;
	__mlibc_uint32 __d3;
	__mlibc_uint32 __d4;
	__mlibc_uint32 __d5;
	__mlibc_uint32 __d6;
	__mlibc_uint32 __d7;
	__mlibc_uint32 __a2;
	__mlibc_uint32 __a3;
	__mlibc_uint32 __a4;
	__mlibc_uint32 __a5;
	__mlibc_uint32 __a6;
	__mlibc_uint32 __a7;
	__mlibc_uint32 __sp;
	__mlibc_uint32 __pc;
};
#elif defined (__loongarch64)
struct __mlibc_jmpbuf_register_state {
	__mlibc_uint64 __ra;
	__mlibc_uint64 __sp;
	__mlibc_uint64 __u0;
	__mlibc_uint64 __s0;
	__mlibc_uint64 __s1;
	__mlibc_uint64 __s2;
	__mlibc_uint64 __s3;
	__mlibc_uint64 __s4;
	__mlibc_uint64 __s5;
	__mlibc_uint64 __s6;
	__mlibc_uint64 __s7;
	__mlibc_uint64 __s8;
	double __fs0;
	double __fs1;
	double __fs2;
	double __fs3;
	double __fs4;
	double __fs5;
	double __fs6;
	double __fs7;
};
#else
#  error "Missing architecture specific code"
#endif

#endif /* _MLIBC_INTERNAL_MACHINE_H */

