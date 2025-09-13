.section .text
.global _start
_start:
	lea main(%rip), %rdi
	mov %rsp, %rsi
	call __mlibc_entry
.section .note.GNU-stack,"",%progbits
