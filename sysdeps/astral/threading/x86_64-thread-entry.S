 .section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
	pop %rdi
	pop %rsi
	pop %rdx
	call __mlibc_thread_trampoline
.section .note.GNU-stack,"",%progbits
