.section .text
.global _start
_start:
	call __mlibc_robu_entry
	ud2

.section .note.GNU-stack,"",%progbits
