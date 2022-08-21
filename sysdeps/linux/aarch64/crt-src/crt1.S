.section .text
.global _start
_start:
	mov x0, sp
	adrp x1, main
	add x1, x1, :lo12:main
	bl __mlibc_entry
.section .note.GNU-stack,"",%progbits

