.section .text
.global _start
_start:
	mov %rsp, %rdi
	mov main@GOTPCREL(%rip), %rsi
	call *__mlibc_entry@GOTPCREL(%rip)

.section .note.GNU-stack,"",%progbits
