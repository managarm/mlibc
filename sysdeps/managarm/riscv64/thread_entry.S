.section .text
.global __mlibc_start_thread
.type __mlibc_start_thread, "function"
__mlibc_start_thread:
	// if this works i am going to eat my dog
	ld a0, 0(sp)
	ld a1, 8(sp)
	ld a2, 16(sp)
	addi sp, sp, 24
	andi sp, sp, -16
	call __mlibc_enter_thread
	unimp

.parent:
	ret
.section .note.GNU-stack,"",%progbits

