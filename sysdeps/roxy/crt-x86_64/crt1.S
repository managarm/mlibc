.section .text
.global _start
.type _start, @function
_start:
	mov %rsp, %rdi
	mov $main, %rsi
	call __mlibc_entry
	ud2
.size _start, . - _start

.section .note.GNU-stack,"",@progbits
