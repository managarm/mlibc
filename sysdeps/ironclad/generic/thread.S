#if defined(__x86_64__)
.section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
	pop %rdi
	pop %rsi
	pop %rdx
	call __mlibc_thread_trampoline
#elif (defined(__riscv) && __riscv_xlen == 64)
.section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
#else
#error "Missing architecture specific code."
#endif

.section .note.GNU-stack,"",%progbits
