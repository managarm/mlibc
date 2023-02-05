#ifndef MLIBC_FENV_H
#define MLIBC_FENV_H

#if defined(__x86_64__)

#define FE_DIVBYZERO 4
#define FE_INEXACT 32
#define FE_INVALID 1
#define FE_OVERFLOW 8
#define FE_UNDERFLOW 16

#define FE_ALL_EXCEPT 63

#define FE_TONEAREST 0
#define FE_DOWNWARD 0x400
#define FE_UPWARD 0x800
#define FE_TOWARDZERO 0xC00

#elif defined(__aarch64__)

#define FE_INVALID 1
#define FE_DIVBYZERO 2
#define FE_OVERFLOW 4
#define FE_UNDERFLOW 8
#define FE_INEXACT 16

#define FE_ALL_EXCEPT 31

#define FE_TONEAREST 0
#define FE_UPWARD 0x400000
#define FE_DOWNWARD 0x800000
#define FE_TOWARDZERO 0xC00000

#elif defined(__riscv) && __riscv_xlen == 64

#define FE_INEXACT 1
#define FE_UNDERFLOW 2
#define FE_OVERFLOW 4
#define FE_DIVBYZERO 8
#define FE_INVALID 16

#define FE_ALL_EXCEPT 31

#define FE_TONEAREST 0
#define FE_TOWARDZERO 1
#define FE_DOWNWARD 2
#define FE_UPWARD 3

#else
#error Unknown architecture
#endif

#endif // MLIBC_FENV_H
