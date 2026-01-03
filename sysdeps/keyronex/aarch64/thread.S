
.section .text
.global __mlibc_thread_entry
__mlibc_thread_entry:
	ldp x0, x1, [sp]
	ldr x2, [sp, #16]
	add sp, sp, #32
	bl __mlibc_thread_trampoline

.section .note.GNU-stack,"",%progbits
