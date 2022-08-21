
.section .text
.global _start
_start:
	adrp x0, main
	add x0, x0, :lo12:main
	bl __mlibc_entry

.section .note.GNU-stack,"",%progbits

