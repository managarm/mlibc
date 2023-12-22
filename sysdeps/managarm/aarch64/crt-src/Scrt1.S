.section .text
.global _start
_start:
	adr x0, main

	bl __mlibc_entry
	brk #0

.section .note.GNU-stack,"",%progbits
