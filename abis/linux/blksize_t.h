
#ifndef _ABIBITS_BLKSIZE_T_H
#define _ABIBITS_BLKSIZE_T_H

#if defined(__x86_64__) || defined(__i386__) || defined(__m68k__)
typedef long blksize_t;
#elif defined(__riscv) || defined(__aarch64__) || defined(__loongarch64)
typedef int blksize_t;
#endif

#endif /* _ABIBITS_BLKSIZE_T_H */

