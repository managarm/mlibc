
.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	ldp x0, x1, [sp], #16
	ldr x2, [sp], #16
	bl __mlibc_enter_thread

.section .note.GNU-stack,"",%progbits

