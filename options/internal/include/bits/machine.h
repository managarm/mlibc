
#ifndef MLIBC_MACHINE_H
#define MLIBC_MACHINE_H

#include <stdint.h>

#if defined (__i386__)
struct __mlibc_jmpbuf_register_state {
	uint32_t ebx;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t esp;
	uint32_t eip;
};
#elif defined (__x86_64__)
struct __mlibc_jmpbuf_register_state {
	uint64_t rbx;
	uint64_t rbp;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;
	uint64_t rsp;
	uint64_t rip;
};
#elif defined (__aarch64__)
struct __mlibc_jmpbuf_register_state {
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t x29;
	uint64_t x30;
	uint64_t sp;
	uint64_t pad;
	uint64_t d8;
	uint64_t d9;
	uint64_t d10;
	uint64_t d11;
	uint64_t d12;
	uint64_t d13;
	uint64_t d14;
	uint64_t d15;
};
#elif defined (__riscv) && __riscv_xlen == 64
struct __mlibc_jmpbuf_register_state {
	uint64_t ra;
	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	uint64_t s9;
	uint64_t s10;
	uint64_t s11;
	uint64_t sp;
	double fs0;
	double fs1;
	double fs2;
	double fs3;
	double fs4;
	double fs5;
	double fs6;
	double fs7;
	double fs8;
	double fs9;
	double fs10;
	double fs11;
};
#elif defined (__m68k__)
struct __mlibc_jmpbuf_register_state {
	uint32_t d2;
	uint32_t d3;
	uint32_t d4;
	uint32_t d5;
	uint32_t d6;
	uint32_t d7;
	uint32_t a2;
	uint32_t a3;
	uint32_t a4;
	uint32_t a5;
	uint32_t a6;
	uint32_t a7;
	uint32_t sp;
	uint32_t pc;
};
#elif defined (__loongarch64)
struct __mlibc_jmpbuf_register_state {
	uint64_t ra;
	uint64_t sp;
	uint64_t u0;
	uint64_t s0;
	uint64_t s1;
	uint64_t s2;
	uint64_t s3;
	uint64_t s4;
	uint64_t s5;
	uint64_t s6;
	uint64_t s7;
	uint64_t s8;
	double fs0;
	double fs1;
	double fs2;
	double fs3;
	double fs4;
	double fs5;
	double fs6;
	double fs7;
};
#else
#  error "Missing architecture specific code"
#endif

#endif /* MLIBC_MACHINE_H */

