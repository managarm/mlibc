
extern "C" {
#ifdef __riscv
#define _SYSCALL_DEF(__num, __enum, __name, __returns, ...)                                        \
	__attribute__((naked)) void __name() { asm volatile("li a7, %0; ecall; ret" ::"i"(__num)); }
#include <abi-bits/syscall_defs.inc>
#endif
}
