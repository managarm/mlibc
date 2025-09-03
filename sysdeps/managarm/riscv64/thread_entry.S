.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	ld a0, 0x0(sp)
	ld a1, 0x8(sp)
	ld a2, 0x10(sp)

	addi sp, sp, 24
	call __mlibc_enter_thread

.section .note.GNU-stack,"",%progbits
