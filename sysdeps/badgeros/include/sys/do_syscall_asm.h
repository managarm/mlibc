
#ifdef __riscv
#define _DO_SYSCALL_ASM(sysno) asm volatile("li a7, " #sysno "; ecall; ret")
#endif
