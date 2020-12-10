
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
	uint64_t x16;
};
#else
#  error "Missing architecture specific code"
#endif

#endif // MLIBC_MACHINE_H

