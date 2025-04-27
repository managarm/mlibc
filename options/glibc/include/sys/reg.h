#ifndef _SYS_REG_H
#define _SYS_REG_H

#ifdef __x86_64__
#define R15 0
#define R14 1
#define R13 2
#define R12 3
#define RBP 4
#define RBX 5
#define R11 6
#define R10 7
#define R9 8
#define R8 9
#define RAX 10
#define RCX 11
#define RDX 12
#define RSI 13
#define RDI 14
#define ORIG_RAX 15
#define RIP 16
#define CS 17
#define EFLAGS 18
#define RSP 19
#define SS 20
#define FS_BASE 21
#define GS_BASE 22
#define DS 23
#define ES 24
#define FS 25
#define GS 26
#elif !(defined(__i386__) || defined(__riscv) || defined(__aarch64__) || defined(__m68k__) || defined(__loongarch64))
#error "Missing architecture specific code."
#endif

#endif
