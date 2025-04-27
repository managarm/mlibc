#ifndef MLIBC_FENV_H
#define MLIBC_FENV_H

#if defined(__x86_64__) || defined(__i386__)

#define FE_DENORMAL 2
#define FE_DIVBYZERO 4
#define FE_INEXACT 32
#define FE_INVALID 1
#define FE_OVERFLOW 8
#define FE_UNDERFLOW 16

#define FE_ALL_EXCEPT (FE_DENORMAL | FE_DIVBYZERO | FE_INEXACT | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

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

#elif defined (__m68k__)

#if __HAVE_68881__ || __mcffpu__ || __HAVE_FPU_

#define FE_INEXACT    8
#define FE_DIVBYZERO  16
#define FE_UNDERFLOW  32
#define FE_OVERFLOW   64
#define FE_INVALID    128

#define FE_ALL_EXCEPT 0xf8

#define FE_TONEAREST  0
#define FE_TOWARDZERO 16
#define FE_DOWNWARD   32
#define FE_UPWARD     48

#else

#define FE_ALL_EXCEPT 0
#define FE_TONEAREST  0

#endif

#elif defined(__loongarch64)

#define FE_INEXACT 0x010000
#define FE_UNDERFLOW 0x020000
#define FE_OVERFLOW 0x040000
#define FE_DIVBYZERO 0x080000
#define FE_INVALID 0x100000

#define FE_ALL_EXCEPT (FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

#define FE_TONEAREST 0x000
#define FE_TOWARDZERO 0x100
#define FE_UPWARD 0x200
#define FE_DOWNWARD 0x300

#else
#error Unknown architecture
#endif

#endif /* MLIBC_FENV_H */
