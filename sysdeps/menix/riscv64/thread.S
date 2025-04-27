.section .text
.global __mlibc_start_thread
__mlibc_start_thread:
	ld a0, 0(sp)
	ld a1, 8(sp)
	ld a2, 16(sp)
	addi sp, sp, 16
	andi sp, sp, -16
	call __mlibc_enter_thread
	unimp

.section .note.GNU-stack,"",%progbits
