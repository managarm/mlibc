.section .text
.global _start
_start:
	mov %rsp, %rdi
	mov $main, %rsi
	call __mlibc_entry
.section .note.GNU-stack,"",%progbits

