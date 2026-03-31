.section .text
.global _start
_start:
	mov x0, sp
	adr x1, main

	bl __mlibc_entry
	brk #0

.section .note.GNU-stack,"",%progbits
