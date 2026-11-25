
.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	mov x9, sp
	ldp x0, x1, [x9]
	ldr x2, [x9, #16]
	add x9, x9, #24
	and sp, x9, #-16
	bl __mlibc_enter_thread

.section .note.GNU-stack,"",%progbits

