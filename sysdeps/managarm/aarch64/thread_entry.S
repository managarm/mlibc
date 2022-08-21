
.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	ldp x0, x1, [sp]
	ldr x2, [sp, #16]
	add sp, sp, #24
	bl __mlibc_enter_thread

.section .note.GNU-stack,"",%progbits

