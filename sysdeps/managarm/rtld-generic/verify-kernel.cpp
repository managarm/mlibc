#include <hel-syscalls.h>
#include <hel.h>

#include <mlibc/all-sysdeps.hpp>

namespace mlibc {

// managarm reads the syscall number from rdi/x0/a0 and returns the HelError in the same
// register, while Linux dispatches on rax/x8/a7 instead. We issue a cheap no-op on both kernels
// (helNop() on managarm, getpid() on Linux) and exit early if we're on a foreign system.
void Sysdeps<VerifyKernel>::operator()() {
	HelWord probe = kHelCallNop;
#if defined(__x86_64__)
	register HelWord linuxScNum asm("rax") = 39; // __NR_getpid
	asm volatile("syscall" : "+D"(probe), "+r"(linuxScNum) : : "rcx", "r11", "rbx", "memory");
#elif defined(__aarch64__)
	register HelWord probeReg asm("x0") = probe;
	register HelWord linuxScNum asm("x8") = 172; // __NR_getpid
	asm volatile("svc 0" : "+r"(probeReg), "+r"(linuxScNum) : : "memory");
	probe = probeReg;
#elif defined(__riscv) && __riscv_xlen == 64
	register HelWord probeReg asm("a0") = probe;
	register HelWord linuxScNum asm("a7") = 172; // __NR_getpid
	asm volatile("ecall" : "+r"(probeReg), "+r"(linuxScNum) : : "memory");
	probe = probeReg;
#else
#error Unknown architecture
#endif

	if (probe == kHelErrNone)
		return;

	// We are not running on managarm; assume that the foreign kernel is Linux. Write a
	// diagnostic to stderr and exit.
	const char msg[] = "rtld: this executable requires a managarm kernel\n";
#if defined(__x86_64__)
	register HelWord scNum asm("rax") = 1; // __NR_write
	register HelWord arg0 asm("rdi") = 2;  // stderr
	register HelWord arg1 asm("rsi") = reinterpret_cast<HelWord>(msg);
	register HelWord arg2 asm("rdx") = sizeof(msg) - 1;
	asm volatile("syscall"
	             : "+r"(scNum)
	             : "r"(arg0), "r"(arg1), "r"(arg2)
	             : "rcx", "r11", "memory");
	scNum = 231; // __NR_exit_group
	arg0 = 127;
	asm volatile("syscall" : "+r"(scNum) : "r"(arg0) : "rcx", "r11", "memory");
#elif defined(__aarch64__)
	register HelWord scNum asm("x8") = 64; // __NR_write
	register HelWord arg0 asm("x0") = 2;   // stderr
	register HelWord arg1 asm("x1") = reinterpret_cast<HelWord>(msg);
	register HelWord arg2 asm("x2") = sizeof(msg) - 1;
	asm volatile("svc 0" : "+r"(arg0) : "r"(scNum), "r"(arg1), "r"(arg2) : "memory");
	scNum = 94; // __NR_exit_group
	arg0 = 127;
	asm volatile("svc 0" : "+r"(arg0) : "r"(scNum) : "memory");
#elif defined(__riscv) && __riscv_xlen == 64
	register HelWord scNum asm("a7") = 64; // __NR_write
	register HelWord arg0 asm("a0") = 2;   // stderr
	register HelWord arg1 asm("a1") = reinterpret_cast<HelWord>(msg);
	register HelWord arg2 asm("a2") = sizeof(msg) - 1;
	asm volatile("ecall" : "+r"(arg0) : "r"(scNum), "r"(arg1), "r"(arg2) : "memory");
	scNum = 94; // __NR_exit_group
	arg0 = 127;
	asm volatile("ecall" : "+r"(arg0) : "r"(scNum) : "memory");
#endif
	// exit_group() does not return on Linux; if we still get here, we are on an unknown kernel.
	__builtin_trap();
}

} // namespace mlibc
