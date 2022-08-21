.section .text
.global _start
_start:
	lea main(%rip), %rdi
	call __mlibc_entry
.section .note.GNU-stack,"",%progbits

